#ifndef CONFIG_LOADER_HH
#define CONFIG_LOADER_HH_HH

#include <fstream>
#include <string>
#include <map>

#include "turingMachine.hh"

class ConfigLoader {
public:
	using char_type = char;

	ConfigLoader();
	~ConfigLoader();

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader(ConfigLoader&&) = delete;
	ConfigLoader& operator =(const ConfigLoader&) = delete;
	ConfigLoader& operator =(ConfigLoader&&) = delete;

	void loadConfig(const TuringMachine& turingMachine, const std::string& fileName = "");
private:
	const std::string defaultFileName_{"config.tm"};
	std::string fileName_;
	std::ifstream configFile_;
	TuringMachine::Instructions_ instructions_;
	TuringMachine::Alphabet_ alphabet_;
	TuringMachine::State_ initialState_;
	TuringMachine::Memory_ memory_;

	const size_t tabSize_ = 4;
	size_t newlinesCount_;
	size_t charCount_;
	size_t pos_;
	std::string buffer_;
protected:
	void incr();
	void eat_spaces();
	bool is_valid_char(char_type c);

	TuringMachine::Moves_ parseMove();
	char_type parseID();
	TuringMachine::Symbol_ parseSymbol();
	TuringMachine::State_ parseState();
	TuringMachine::Instructions_type_ parseTuple();
	void parseInstructionList();
	void parseSymbolsListAlphabet();
	void parseSymbolsListMemory();
	void parseOption();
	void parseElements();
	std::string get_error(const std::string& msg);
};

#endif // ! CONFIG_LOADER_HH