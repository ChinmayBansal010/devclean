#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct CacheDefinition
{
    std::string name;
    std::vector<std::string> aliases;
    std::filesystem::path linuxPath;
    std::filesystem::path windowsPath;
    std::string category;
    std::string description;
    std::vector<std::string> environmentVariables;
    std::vector<std::string> osSupport;
    int priority = 0;
    bool enabled = true;
    std::vector<std::filesystem::path> cachePaths;
};
