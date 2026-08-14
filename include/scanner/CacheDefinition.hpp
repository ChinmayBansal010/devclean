#pragma once

#include <filesystem>
#include <string>
#include <utility>
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

    CacheDefinition() = default;

    CacheDefinition(
        std::string cacheName,
        std::vector<std::string> cacheAliases,
        std::filesystem::path cacheLinuxPath,
        std::filesystem::path cacheWindowsPath,
        std::string cacheCategory,
        std::string cacheDescription,
        std::vector<std::string> cacheEnvironmentVariables,
        std::vector<std::string> cacheOsSupport,
        int cachePriority,
        bool cacheEnabled = true,
        std::vector<std::filesystem::path> paths = {}
    )
        : name(std::move(cacheName)),
          aliases(std::move(cacheAliases)),
          linuxPath(std::move(cacheLinuxPath)),
          windowsPath(std::move(cacheWindowsPath)),
          category(std::move(cacheCategory)),
          description(std::move(cacheDescription)),
          environmentVariables(std::move(cacheEnvironmentVariables)),
          osSupport(std::move(cacheOsSupport)),
          priority(cachePriority),
          enabled(cacheEnabled),
          cachePaths(std::move(paths))
    {
    }
};
