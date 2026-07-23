#pragma once

#include "scanner/CacheDefinition.hpp"

#include <string>
#include <vector>

struct AppConfig
{
    int schemaVersion = 3;
    std::vector<std::string> disabledCaches;
    std::vector<std::string> ignoredCaches;
    std::string defaultColor = "none";
    std::string defaultSort = "name";
    std::string defaultCategory;
    std::string version = "1.1";
    std::vector<CacheDefinition> customCaches;
};

class ConfigLoader
{
public:
    static AppConfig load();
    static void save(const AppConfig& config);
};
