#include "../include/configLoader.hh"

ConfigLoader::ConfigLoader() :
	fileName_{defaultFileName_},
	configFile_{defaultFileName_} { }

ConfigLoader::~ConfigLoader() {
	if (configFile_.is_open())
		configFile_.close();
}

void
ConfigLoader::loadConfig(const TuringMachine& turingMachine, const std::string& fileName) {
	
}