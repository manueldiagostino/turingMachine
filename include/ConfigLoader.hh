#ifndef CONFIG_LOADER_HH
#define CONFIG_LOADER_HH

#include <fstream>
#include <string>
#include <map>
#include <iostream>

#include <getopt.h>
#include "TuringMachine.hh"
#include "Colors.hh"

namespace Loaders {
	
class ConfigLoader {
public:
	using char_type = char;

	ConfigLoader();
	~ConfigLoader();

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader(ConfigLoader&&) = delete;
	ConfigLoader& operator =(const ConfigLoader&) = delete;
	ConfigLoader& operator =(ConfigLoader&&) = delete;

	void 
	loadConfig(
		TuringMachine& turingMachine, const std::string& fileName = "",
		int argc = 0, char **argv = nullptr
	);

	void 
	setTabSize(size_t size);

	const struct option long_options[3] = {
        {"tab-width"        , no_argument       , 0,    't'},
        {"head-color"      	, no_argument       , 0,    'c'},
        {"help"		      	, no_argument       , 0,    'h'}
    };

	int 
	parse_cmd(int argc, char **argv, ConfigLoader& cl, TuringMachine& tm);

	void
	print_usage(char **argv);
private:
	const std::string defaultFileName_{"config.tm"};
	std::string fileName_;
	std::ifstream configFile_;
	TuringMachine::Instructions_ instructions_;
	TuringMachine::Alphabet_ alphabet_;
	TuringMachine::States_ states_;
	TuringMachine::State_ initialState_;
	TuringMachine::Memory_ memory_;

	size_t tabSize_ {4};
	size_t newlinesCount_;
	size_t charCount_;
	size_t pos_;
	std::string buffer_;
protected:
	void incr();
	void eat_spaces();
	bool is_valid_char(char_type c);

	TuringMachine::Moves_ parseMove();
	char_type parseChar();
	TuringMachine::Symbol_ parseID();
	TuringMachine::Symbol_ parseSymbol();
	TuringMachine::State_ parseState();
	TuringMachine::Instructions_type_ parseTuple();
	void parseInstructionList();
	void parseSymbolsListAlphabet();
	void parseSymbolsListMemory();
	void parseSymbolsListStates();
	void parseOption();
	void parseElements();

	std::string get_error(const std::string& msg);
	void printDebug(const std::string& msg);
};

} // ! NAMESPACE LOADERS

#endif // ! CONFIG_LOADER_HH