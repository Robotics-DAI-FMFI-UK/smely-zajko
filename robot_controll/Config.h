#ifndef CONFIG_H
#define CONFIG_H

#include "yaml-cpp/yaml.h"

#include <map>
#include <string.h>

using namespace std;

class Config {
public:
    static double getDouble(string key);
    static string getString(string key);
private:
    static YAML::Node config;
    Config(){};
};

#endif /* CONFIG_H */

