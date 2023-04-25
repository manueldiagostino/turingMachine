#ifndef CONFIG_LOADER_HH
#define CONFIG_LOADER_HH_HH

#include <fstream>
#include <string>

#include "turing_machine.hh"

class ConfigLoader {
public:
	ConfigLoader() = default;
	~ConfigLoader();

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader(ConfigLoader&&) = delete;
	ConfigLoader& operator =(const ConfigLoader&) = delete;
	ConfigLoader& operator =(ConfigLoader&&) = delete;

	void loadConfig(const TuringMachine& turingMachine, const std::string& fileName);
private:
	std::string fileName_;
	std::ifstream configFile_;

	const std::string defaultFileName_{"config.tm"};
};

#endif // ! CONFIG_LOADER_HH_HH