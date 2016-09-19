#include "Config.h"

YAML::Node Config::config = YAML::LoadFile("../config.yaml");

string Config::getString(string key) {

    return Config::config[key].as<string>();
}

int Config::getInt(string key) {

    return Config::config[key].as<int>();
}

double Config::getDouble(string key) {

    return Config::config[key].as<double>();
}

bool Config::isDefined(string key){
    
    return Config::config[key].IsDefined();
}