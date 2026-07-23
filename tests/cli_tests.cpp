#include "core/ArgumentParser.hpp"
#include "core/Config.hpp"
#include "scanner/CacheRegistry.hpp"
#include "scanner/PluginLoader.hpp"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    char* argv[] = {
        const_cast<char*>("devclean"),
        const_cast<char*>("clean"),
        const_cast<char*>("--force"),
        const_cast<char*>("--dry-run"),
        const_cast<char*>("python"),
        const_cast<char*>("cargo")
    };

    const ParsedArgs parsed = ArgumentParser::parse(6, argv);
    assert(parsed.command == "clean");
    assert(parsed.force);
    assert(parsed.dryRun);
    assert(parsed.targets.size() == 2);
    assert(parsed.targets[0] == "python");
    assert(parsed.targets[1] == "cargo");

    const auto pythonCaches = CacheRegistry::getMatchingCaches({"python"});
    assert(!pythonCaches.empty());
    assert(std::any_of(pythonCaches.begin(), pythonCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pip";
    }));

    const auto rustCaches = CacheRegistry::getMatchingCaches({"rust"});
    assert(!rustCaches.empty());
    assert(std::any_of(rustCaches.begin(), rustCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "cargo";
    }));

    const auto pythonToolCaches = CacheRegistry::getMatchingCaches({"pipenv"});
    assert(!pythonToolCaches.empty());
    assert(std::any_of(pythonToolCaches.begin(), pythonToolCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pipenv";
    }));

    const auto nodeToolCaches = CacheRegistry::getMatchingCaches({"pnpm"});
    assert(!nodeToolCaches.empty());
    assert(std::any_of(nodeToolCaches.begin(), nodeToolCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "pnpm";
    }));

    const auto buildToolCaches = CacheRegistry::getMatchingCaches({"ccache"});
    assert(!buildToolCaches.empty());
    assert(std::any_of(buildToolCaches.begin(), buildToolCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "ccache";
    }));

    const auto containerCaches = CacheRegistry::getMatchingCaches({"containers"});
    assert(std::any_of(containerCaches.begin(), containerCaches.end(), [](const CacheDefinition& cache) {
        return cache.name == "docker-builder" || cache.name == "docker-volumes" || cache.name == "podman";
    }));

    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        const std::filesystem::path configDir = std::filesystem::path(home) / ".config" / "devclean";
        std::filesystem::create_directories(configDir);
        const std::filesystem::path configPath = configDir / "config.json";
        std::ofstream output(configPath);
        output << R"({"disabled": ["cargo"], "ignored": ["npm"], "default_sort": "size", "schemaVersion": 1})";
        output.close();

        setenv("HOME", home, 1);
        const AppConfig migrated = ConfigLoader::load();
        assert(std::find(migrated.disabledCaches.begin(), migrated.disabledCaches.end(), "cargo") != migrated.disabledCaches.end());
        assert(std::find(migrated.ignoredCaches.begin(), migrated.ignoredCaches.end(), "npm") != migrated.ignoredCaches.end());
        assert(migrated.defaultSort == "size");
        assert(migrated.schemaVersion >= 3);
    }

    const std::filesystem::path tempRoot = std::filesystem::temp_directory_path() / "devclean-cli-tests";
    std::filesystem::create_directories(tempRoot / ".config" / "devclean" / "plugins");
    setenv("HOME", tempRoot.string().c_str(), 1);

    AppConfig customConfig;
    CacheDefinition customCache;
    customCache.name = "custom-cache";
    customCache.category = "build";
    customCache.description = "Custom plugin cache";
    customCache.enabled = true;
    customCache.priority = 7;
    customCache.aliases = {"custom"};
    customCache.environmentVariables = {"DEV_CLEAN_TEST"};
    customCache.osSupport = {"linux", "windows"};
    customCache.linuxPath = tempRoot / "custom-cache";
    customCache.cachePaths = {tempRoot / "custom-cache" / "alt"};
    customConfig.customCaches.push_back(customCache);
    ConfigLoader::save(customConfig);

    const AppConfig reloadedConfig = ConfigLoader::load();
    assert(reloadedConfig.customCaches.size() == 1);
    assert(reloadedConfig.customCaches[0].description == "Custom plugin cache");
    assert(reloadedConfig.customCaches[0].priority == 7);
    assert(reloadedConfig.customCaches[0].environmentVariables.size() == 1);
    assert(reloadedConfig.customCaches[0].osSupport.size() == 2);
    assert(reloadedConfig.customCaches[0].aliases[0] == "custom");
    assert(!reloadedConfig.customCaches[0].cachePaths.empty());

    std::ofstream pluginFile(tempRoot / ".config" / "devclean" / "plugins" / "custom-plugin.json");
    pluginFile << R"({"name":"plugin-cache","category":"build","description":"Loaded from plugin","enabled":true,"priority":5,"aliases":["plugin"],"environmentVariables":["PLUGIN_CACHE"],"osSupport":["linux"],"path":"/tmp/plugin-cache","cachePaths":["/tmp/plugin-cache/alt"]})";
    pluginFile.close();

    const auto loadedPlugins = PluginLoader::getInstance().loadPlugins();
    assert(!loadedPlugins.empty());
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [](const CacheDefinition& cache) {
        return cache.name == "plugin-cache";
    }));
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [](const CacheDefinition& cache) {
        return cache.name == "plugin-cache" && !cache.cachePaths.empty();
    }));

    std::cout << "devclean CLI tests passed" << std::endl;
    return 0;
}
