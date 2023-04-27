#include <sstream>

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
		}

		incr();
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
ConfigLoader::parseAlnum() {
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

std::string
ConfigLoader::parseSymbol() {
	if (std::isspace(buffer_[pos_]))
		throw get_error("Expected an alphanumeric value, found space");

	std::string res(1, parseAlnum());

	if (buffer_[pos_] == ',' || std::isspace(buffer_[pos_])) {
		eat_spaces();
		return res;
	}
	
	return res + parseSymbol();
}

std::string
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
	if (buffer_[pos_] == '}') {
		instructions_.insert(std::move(res));
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
ConfigLoader::parseElements() {
	#ifdef DEBUG
	std::cerr << "Parsing elements" << std::endl;
	#endif
	eat_spaces();

	parseMove();
	parseAlnum();
	// TuringMachine::Instructions_type_ res = parseTuple();
	// std::cerr << res.first.first << ", " << res.first.second << std::endl;
	parseInstructionList();

	#ifdef DEBUG
	std::cerr << "Parsing elements finished" << std::endl;
	#endif
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
		throw get_error("`}` not found, <" + std::string(1,buffer_[pos_]) + "> instead");

	incr();
	parseElements();
	eat_spaces();

	// check '}'
	if (buffer_[pos_] != '}')
		throw get_error("`}` not found, <" + std::string(1,buffer_[pos_]) + "> instead");
	incr();

	if (charCount_ == buffer_.length() || buffer_[charCount_] == '\n') {
		charCount_ = 0;
		++newlinesCount_;
	}
	incr();

	#ifdef DEBUG
	std::cerr << "Parsing over" << std::endl;
	#endif
}
















/* 
void
ConfigLoader::loadTuple(std::pair<TuringMachine::Key_, TuringMachine::Value_>& instruction) {
	if (!configFile_.is_open())
		throw std::string("Config file not opened");

	char angular;
	configFile_ >> std::ws >> angular;

	if (angular != '<')
		throw std::string("Parsing error: expected '<'");

	TuringMachine::State_ currState;
	TuringMachine::Symbol_ currSymbol;
	TuringMachine::State_ nextState;
	TuringMachine::Symbol_ nextSymbol;
	std::string move;
	TuringMachine::Moves_ nextMove;

	getline(configFile_ >> std::ws, currState, ',');
	getline(configFile_ >> std::ws, currSymbol, ',');
	getline(configFile_ >> std::ws, nextState, ',');
	getline(configFile_ >> std::ws, nextSymbol, ',');
	getline(configFile_ >> std::ws, move, '>');

	nextMove = make_move(move);
	
	TuringMachine::Key_ key = std::make_pair(currState, currSymbol);
	TuringMachine::Value_ value = std::make_tuple(nextState, nextSymbol, nextMove);

	instructions_.insert(std::move(std::make_pair(key, value)));
}

void
ConfigLoader::loadInstructions() {
	if (!configFile_.is_open())
		throw std::string("Config file not opened");

	char par;

	// elimino '{'
	configFile_ >> std::ws >> par;
	if (par != '{')
		throw std::string("Parsing error: expected '{'");

	std::pair<TuringMachine::Key_, TuringMachine::Value_> instruction;

	char comma = ',';
	while (comma == ',') {
		loadTuple(instruction);
		instructions_.insert(instruction);
	}

	// elimino '{'
	configFile_ >> std::ws >> par;
	if (par != '}')
		throw std::string("Parsing error: expected '}'");		
}

void
ConfigLoader::loadAlphabet() {

}

void
ConfigLoader::loadConfig(const TuringMachine& turingMachine, const std::string& fileName) {
	if (fileName != "")
		fileName_ = fileName;

	configFile_.open(fileName_);

	const std::string INSTRUCTION = "instructions";
	const std::string ALPHABET = "alphabet";
	const std::string INITIAL_STATE = "initial state";

	std::string key, value;

	while (std::getline(configFile_ >> std::ws, key, '=')) {
		key.erase(std::remove(key.begin(),key.end(),' '),key.end());

		if (key == INSTRUCTION)
			loadInstructions();
		else if (key == ALPHABET)
			loadAlphabet();
	}


} 
*/