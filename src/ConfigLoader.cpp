#include <sstream>
#include <cstring>

#include "../include/ConfigLoader.hh"

using namespace Loaders;

ConfigLoader::ConfigLoader() :
	fileName_{},
	configFile_{},
	tabSize_{4},
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
		res = TuringMachine::Moves_::left;
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
ConfigLoader::parseChar() {
	if (std::isspace(buffer_[pos_]) || buffer_[pos_] == ',')
		return '\0';

	char_type res = buffer_[pos_];
	if (!is_valid_char(buffer_[pos_])) {
		throw get_error("Expected a valid char_type value, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	return res;
}

TuringMachine::Symbol_
ConfigLoader::parseID() {
	std::string res(1, parseChar());

	if (buffer_[pos_] == ',' || buffer_[pos_] == '}' || isspace(buffer_[pos_]))
		return res;
	
	return res + parseID();
}

TuringMachine::Symbol_ 
ConfigLoader::parseSymbol() {
	eat_spaces();
	TuringMachine::Symbol_ res = parseID();
	eat_spaces();
	return res;
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


	TuringMachine::Symbol_ currSymbol = parseID();
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

	TuringMachine::Symbol_ newSymbol = parseID();
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
ConfigLoader::parseSymbolsListStates() {
	eat_spaces();

	states_.insert(std::move(parseSymbol()));

	if (buffer_[pos_] == '}') {
		return;
	}
	else if (buffer_[pos_] != ',') {
		throw get_error("Expected `,`, found <" + 
			std::string(1, buffer_[pos_]) + ">");
	}

	incr();
	eat_spaces();
	parseSymbolsListStates();
}

void
ConfigLoader::parseOption() {
	eat_spaces();

	std::string instr = "instructions";
	std::string alpha = "alphabet";
	std::string mem = "memory";
	std::string states = "states";
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
	else if (!strncmp(buffer_.c_str()+pos_, states.c_str(), states.size())) {
		pos_ += states.size();
		charCount_ += states.size();

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
		parseSymbolsListStates();
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
ConfigLoader::loadConfig(
	TuringMachine& turingMachine, const std::string& fileName,
	int argc, char **argv) {
	
	if (argc != 0) {
		parse_cmd(argc, argv, *this, turingMachine);
	}

	if (fileName != "")
		fileName_ = fileName;

	configFile_.open(fileName_);
	if (!configFile_.is_open())
		throw ("<" + fileName_ + ">" + ": no such file\n");

	#ifdef DEBUG
	printDebug("File opened");
	#endif

	std::stringstream b;
	b << configFile_.rdbuf();
	buffer_ = b.str();

	#ifdef DEBUG
	printDebug("buffer_ opened");
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

	turingMachine.setInitialState(std::move(initialState_));
	turingMachine.setAlphabet(std::move(alphabet_));
	turingMachine.setStates(std::move(states_));
	turingMachine.setInitialState(std::move(initialState_));
	turingMachine.setInstructions(std::move(instructions_));
	turingMachine.setMemory(std::move(memory_));

	#ifdef DEBUG
	turingMachine.print_all();
	#endif
}

void 
ConfigLoader::setTabSize(size_t size) {
	tabSize_ = size;
}


void
ConfigLoader::printDebug(const std::string& msg) {
	std::cerr 
		<< Color::Modifier(Color::FG_GREEN) << "[DEBUG]:\t"
		<< msg 
		<< Color::Modifier(Color::FG_DEFAULT) << std::endl;
}

void
ConfigLoader::print_usage(char **argv) {
	std::cerr << "Usage: " << argv[0] << " [Options] " << std::endl;
	// std::cerr << "\t-v, --verbose:   \tperform a step-by-step computation" 							<< std::endl;
	
	std::cerr << "\n[Options]" << std::endl;
	std::cerr << "\t-c, --head-color:\tchanges head's highlighting color; possible ones are" 		<< std::endl;
	std::cerr << "\t                 \t  - blue" 		<< std::endl;
	std::cerr << "\t                 \t  - green" 		<< std::endl;
	std::cerr << "\t                 \t  - magenta" 	<< std::endl;
	std::cerr << "\t                 \t  - red" 		<< std::endl;
	std::cerr << "\t                 \t  - white" 	<< std::endl;
	std::cerr << "\t                 \t  - yellow" 	<< std::endl;
	std::cerr << "\t-t, --tab-width: \tset the tab width (default is 4); useful in the DEBUG mode" 	<< std::endl;
}

int 
ConfigLoader::parse_cmd(int argc, char **argv, ConfigLoader& cl, TuringMachine& tm) {
	int option_index = 0;

	int c;
    std::string color;
    while ((c = getopt_long(argc, argv, ":t:c:", long_options, &option_index)) != -1) {
        switch (c) {
        /* case 'v':
			#ifndef DEBUG
			#define DEBUG 1
			#endif
			printDebug("Verbose mod on");
            break; */
        case 'c':
            color = std::string(optarg);
			tm.setBackgroundColor(Color::BG_Converter(color));

			#ifdef DEBUG
			printDebug("Highlight color set to " + color);
			#endif
            break;
        case 't':
			c = atoi(optarg);
            cl.setTabSize(c);

			#ifdef DEBUG
			printDebug("Tab size set to " + std::to_string(c));
			#endif
            break;
        case ':':
            std::cerr << Color::MD_FG_RED
			<< "Option -" << (char)optopt << " needs an argument." 
			<< Color::MD_FG_DEFAULT << std::endl;
            print_usage(argv);
            return 1;
        case '?':
            std::cerr << "Unknown option";
            if (optopt)
                std::cerr << " -" << (char)optopt << std::endl;
            else
                std::cerr << std::endl;

            print_usage(argv);
            return 1;
        default:
            break;
        }
    }

    return 0;
}
