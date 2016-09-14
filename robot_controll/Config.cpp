#include "Config.h"

YAML::Node Config::config = YAML::LoadFile("../config.yaml");

string Config::getString(string key) {

    return Config::config[key].as<string>();
}

double Config::getDouble(string key) {

    return Config::config[key].as<double>();
}