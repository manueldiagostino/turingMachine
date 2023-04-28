#include <sstream>
#include <cstring>

#include "../include/configLoader.hh"

ConfigLoader::ConfigLoader() :
	fileName_{},
	configFile_{},
	newlinesCount_{0},
	charCount_{0},
	pos_{0},
	buffer_{} {
	
	fileName_ = defaultFileName_;
}

ConfigLoader::~ConfigLoader() {
	if (configFile_.is_open())
		configFile_.close();
}

inline
std::string
ConfigLoader::get_error(const std::string& msg) {
	std::string res = "\tError at line " + std::to_string(newlinesCount_+1);
	res += ", column " + std::to_string(charCount_+1);
	return res += ": " + msg;
}

inline
void
ConfigLoader::eat_spaces() {
	while (std::isspace(buffer_[pos_])) {
		
		if (buffer_[pos_] == '\n') {
			++newlinesCount_;
			charCount_ = 0;
			++pos_;
			continue;
		}
		else if (buffer_[pos_] == '\t') {
			charCount_ += tabSize_; // incremento di 1 in meno, chiamo anche incr()
			++pos_;
			continue;
		}
		else
			incr(); // incrementa anche charCount_
	}
}

inline
void
ConfigLoader::incr() {
	++pos_;
	++charCount_;
}

inline
bool
ConfigLoader::is_valid_char(char_type c) {
	return std::isalnum(c) || c == '$' || c == '#' || c == '&' || c == '%';
}

TuringMachine::Moves_
ConfigLoader::parseMove() {
	eat_spaces();
	TuringMachine::Moves_ res;

	if (buffer_[pos_] == 'R' || buffer_[pos_] == 'r') {
		res = TuringMachine::Moves_::right;
	}
	else if (buffer_[pos_] == 'L' || buffer_[pos_] == 'l') {
		res = TuringMachine::Moves_::right;
	}
	else {
		throw get_error("Invalid move <" + 
			std::string(1,(char)(buffer_[pos_])) + ">");
	}

	incr();
	eat_spaces();
	return res;
}

ConfigLoader::char_type
ConfigLoader::parseID() {
	if (std::isspace(buffer_[pos_]))
		throw get_error("Expected an alphanumeric value, found space");

	char_type res = buffer_[pos_];
	if (!is_valid_char(buffer_[pos_])) {
		throw get_error("Expected an alphanumeric value, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	return res;
}

TuringMachine::Symbol_
ConfigLoader::parseSymbol() {
	if (std::isspace(buffer_[pos_]))
		throw get_error("Expected an alphanumeric value, found space");

	std::string res(1, parseID());

	if (buffer_[pos_] == ',' || std::isspace(buffer_[pos_])) {
		eat_spaces();
		return res;
	}
	
	return res + parseSymbol();
}

TuringMachine::State_
ConfigLoader::parseState() {
	return parseSymbol();
}

TuringMachine::Instructions_type_ 
ConfigLoader::parseTuple() {
	eat_spaces();

	if (buffer_[pos_] != '<') {
		throw get_error("Expected `<`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}
	incr();
	eat_spaces();

	TuringMachine::State_ currState = parseState();
	if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}
	incr();
	eat_spaces();


	TuringMachine::Symbol_ currSymbol = parseSymbol();
	if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}
	incr();
	eat_spaces();

	TuringMachine::State_ newState = parseState();
	if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}
	incr();
	eat_spaces();

	TuringMachine::Symbol_ newSymbol = parseSymbol();
	if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}
	incr();
	eat_spaces();

	TuringMachine::Moves_ move = parseMove();
	if (buffer_[pos_] != '>') {
		throw get_error("Expected `>`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}
	incr();
	eat_spaces();

	TuringMachine::Key_ key = std::make_pair(currState, currSymbol);
	TuringMachine::Value_ value = std::make_tuple(newState, newSymbol, move);
	TuringMachine::Instructions_type_ res = std::make_pair(key, value);

	return res;
}

void 
ConfigLoader::parseInstructionList() {
	eat_spaces();

	TuringMachine::Instructions_type_ res = parseTuple();
	instructions_.insert(std::move(res));
	if (buffer_[pos_] == '}') {
		return;
	}
	else if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	eat_spaces();
	parseInstructionList();
}


void 
ConfigLoader::parseSymbolsListAlphabet() {
	eat_spaces();

	alphabet_.insert(std::move(parseSymbol()));

	if (buffer_[pos_] == '}') {
		return;
	}
	else if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	eat_spaces();
	parseSymbolsListAlphabet();
}

void 
ConfigLoader::parseSymbolsListMemory() {
	eat_spaces();

	memory_.push_back(std::move(parseSymbol()));

	if (buffer_[pos_] == '}') {
		return;
	}
	else if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	eat_spaces();
	parseSymbolsListMemory();
}



void
ConfigLoader::parseOption() {
	eat_spaces();

	std::string instr = "instructions";
	std::string alpha = "alphabet";
	std::string mem = "memory";
	std::string initState = "initial_state";

	if (!strncmp(buffer_.c_str()+pos_, instr.c_str(), instr.size())) {
		pos_ += instr.size();
		charCount_ += instr.size();

		eat_spaces();
		if (buffer_[pos_] != '=')
			throw get_error("Expected `=`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();
		eat_spaces();

		if (buffer_[pos_] != '{')
			throw get_error("Expected `{`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();

		parseInstructionList();
	}
	else if (!strncmp(buffer_.c_str()+pos_, alpha.c_str(), alpha.size())) {
		pos_ += alpha.size();
		charCount_ += alpha.size();

		eat_spaces();
		if (buffer_[pos_] != '=')
			throw get_error("Expected `=`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();
		eat_spaces();

		if (buffer_[pos_] != '{')
			throw get_error("Expected `{`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();

		parseSymbolsListAlphabet();
	}
	else if (!strncmp(buffer_.c_str()+pos_, mem.c_str(), mem.size())) {
		pos_ += mem.size();
		charCount_ += mem.size();

		eat_spaces();
		if (buffer_[pos_] != '=')
			throw get_error("Expected `=`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();
		eat_spaces();

		if (buffer_[pos_] != '{')
			throw get_error("Expected `{`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();

		parseSymbolsListMemory();
	}
	else if (!strncmp(buffer_.c_str()+pos_, initState.c_str(), initState.size())) {
		pos_ += initState.size();
		charCount_ += initState.size();

		eat_spaces();
		if (buffer_[pos_] != '=')
			throw get_error("Expected `=`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();
		eat_spaces();

		if (buffer_[pos_] != '{')
			throw get_error("Expected `{`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

		incr();
		initialState_ = parseState();
	}
	else
		throw get_error("Unknown option");

	if (buffer_[pos_] != '}')
			throw get_error("Expected `{`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

	incr();
	eat_spaces();
}







void
ConfigLoader::parseElements() {
	eat_spaces();
	parseOption();

	if (buffer_[pos_] == '}') {
		return;
	}
	else if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	eat_spaces();
	parseElements();
}

void
ConfigLoader::loadConfig(const TuringMachine& turingMachine, const std::string& fileName) {
	if (fileName != "")
		fileName_ = fileName;

	configFile_.open(fileName_);
	if (!configFile_.is_open())
		throw ("<" + fileName_ + ">" + ": no such file\n");

	#ifdef DEBUG
	std::cerr << "File opened" << std::endl;
	#endif

	std::stringstream b;
	b << configFile_.rdbuf();
	buffer_ = b.str();

	#ifdef DEBUG
	std::cerr << "buffer_ opened" << std::endl;
	#endif
	
	eat_spaces();

	// check '{'
	if (buffer_[pos_] != '{')
		throw get_error("Expected `{`, found <" + 
			std::string(1, buffer_[pos_]) + ">");

	incr();
	parseElements();

	// check '}'
	if (buffer_[pos_] != '}')
		throw get_error("Expected `}`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	incr();

	if (charCount_ == buffer_.length() || buffer_[charCount_] == '\n') {
		charCount_ = 0;
		++newlinesCount_;
	}
	incr(); // THE END

	#ifdef DEBUG
	std::cerr << "Parsing over" << std::endl;
	std::cerr << "Alphabet: " << alphabet_ << std::endl;
	std::cerr << "Instructions: " << instructions_ << std::endl;
	std::cerr << "Memory: " << memory_ << std::endl;
	#endif
}