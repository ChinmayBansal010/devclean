#include "cleaner/CleanEngine.hpp"
#include "core/ArgumentParser.hpp"
#include "core/Config.hpp"
#include "platform/Filesystem.hpp"
#include "scanner/CacheRegistry.hpp"
#include "scanner/PluginLoader.hpp"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

namespace {

void setEnvVar(const char* name, const std::string& value)
{
#ifdef _WIN32
    _putenv_s(name, value.c_str());

    if (std::string(name) == "HOME")
        _putenv_s("USERPROFILE", value.c_str());
#else
    setenv(name, value.c_str(), 1);
#endif
}

std::string getHomeEnv()
{
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    return home == nullptr ? std::string() : std::string(home);
}

} // namespace

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

        setEnvVar("HOME", home);
        const AppConfig migrated = ConfigLoader::load();
        assert(std::find(migrated.disabledCaches.begin(), migrated.disabledCaches.end(), "cargo") != migrated.disabledCaches.end());
        assert(std::find(migrated.ignoredCaches.begin(), migrated.ignoredCaches.end(), "npm") != migrated.ignoredCaches.end());
        assert(migrated.defaultSort == "size");
        assert(migrated.schemaVersion >= 3);
    }

    const auto tempRoot =
        std::filesystem::temp_directory_path() /
        ("devclean-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    setEnvVar("HOME", tempRoot.string());

    #ifdef _WIN32
    setEnvVar("USERPROFILE", tempRoot.string());
    #endif

    std::filesystem::create_directories(
    tempRoot / ".config" / "devclean" / "plugins");

    std::filesystem::create_directories(
        tempRoot / "plugin-cache" / "alt");

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

    const auto pluginJson =
        tempRoot / ".config" / "devclean" / "plugins" / "custom-plugin.json";

    std::ofstream pluginFile(pluginJson);
    assert(pluginFile.is_open());

    const auto pluginPath = tempRoot / "plugin-cache";
    const auto pluginAlt = pluginPath / "alt";

    pluginFile
        << "{"
        << "\"name\":\"plugin-cache\","
        << "\"category\":\"build\","
        << "\"description\":\"Loaded from plugin\","
        << "\"enabled\":true,"
        << "\"priority\":5,"
        << "\"aliases\":[\"plugin\"],"
        << "\"environmentVariables\":[\"PLUGIN_CACHE\"],"
        << "\"osSupport\":[\"linux\"],"
        << "\"path\":\"" << pluginPath.string() << "\","
        << "\"cachePaths\":[\"" << pluginAlt.string() << "\"]"
        << "}";

    pluginFile.close();

    assert(std::filesystem::exists(pluginJson));

    const auto loadedPlugins = PluginLoader::getInstance().loadPlugins();
    
    assert(!loadedPlugins.empty());
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [](const CacheDefinition& cache) {
        return cache.name == "plugin-cache";
    }));
    assert(std::any_of(loadedPlugins.begin(), loadedPlugins.end(), [](const CacheDefinition& cache) {
        return cache.name == "plugin-cache" && !cache.cachePaths.empty();
    }));

    std::ofstream invalidPluginFile(tempRoot / ".config" / "devclean" / "plugins" / "invalid-plugin.json");
    invalidPluginFile << R"({"name":"invalid/plugin","category":"build","path":"../escape"})";
    invalidPluginFile.close();
    const auto loadedAfterInvalid = PluginLoader::getInstance().loadPlugins();
    assert(std::none_of(loadedAfterInvalid.begin(), loadedAfterInvalid.end(), [](const CacheDefinition& cache) {
        return cache.name == "invalid/plugin";
    }));

    assert(!Filesystem::isProtectedPath(tempRoot / "custom-cache"));
    const std::string homePath = getHomeEnv();
    if (!homePath.empty())
        assert(Filesystem::isProtectedPath(homePath));

    std::cout << "HOME = " << std::getenv("HOME") << std::endl;

    #ifdef _WIN32
    std::cout << "USERPROFILE = " << std::getenv("USERPROFILE") << std::endl;
    #endif

    std::cout << "homePath = " << homePath << std::endl;
    CleanEngine cleaner;
    const auto protectedRemoval = cleaner.removeDirectory(homePath.empty() ? std::filesystem::path("/") : std::filesystem::path(homePath));
    assert(!protectedRemoval.success);
    assert(!protectedRemoval.error.empty());

    const auto safeCacheDir = tempRoot / "safe-cache";
    std::filesystem::create_directories(safeCacheDir / "nested");
    std::ofstream(safeCacheDir / "nested" / "artifact.txt") << "cache";
    const auto removed = cleaner.removeDirectory(safeCacheDir);
    assert(removed.success);
    assert(!std::filesystem::exists(safeCacheDir));
    std::filesystem::remove_all(tempRoot);
    std::cout << "devclean CLI tests passed" << std::endl;
    return 0;
}
