#ifndef CONFIG_H
#define CONFIG_H

#include "yaml-cpp/yaml.h"

#include <string.h>

using namespace std;

class Config {
public:
    static double getDouble(string key);
    static int getInt(string key);
    static string getString(string key);
    static bool isDefined(string key);
private:
    static YAML::Node config;
    Config(){};
};

#endif /* CONFIG_H */

