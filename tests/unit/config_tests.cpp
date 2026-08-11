#include "core/Config.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace {

void setEnv(const char* name, const std::string& value)
{
#ifdef _WIN32
    _putenv_s(name, value.c_str());
#else
    setenv(name, value.c_str(), 1);
#endif
}

}

int main()
{
    const auto root = std::filesystem::temp_directory_path() /
        ("devclean-config-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));

    std::filesystem::create_directories(root);
    setEnv("HOME", root.string());
#ifdef _WIN32
    setEnv("USERPROFILE", root.string());
    setEnv("APPDATA", (root / "AppData" / "Roaming").string());
#endif

    AppConfig config;
    config.disabledCaches = {"cargo"};
    config.ignoredCaches = {"npm"};
    config.defaultSort = "size";
    config.defaultCategory = "build";

    CacheDefinition custom;
    custom.name = "test-cache";
    custom.category = "build";
    custom.description = "Configuration test cache";
    custom.enabled = true;
    custom.priority = 3;
    custom.aliases = {"test"};
    custom.environmentVariables = {"TEST_CACHE_ROOT"};
    custom.cachePaths = {root / "cache"};
    config.customCaches.push_back(custom);

    ConfigLoader::save(config);
    const AppConfig loaded = ConfigLoader::load();

    assert(std::find(loaded.disabledCaches.begin(), loaded.disabledCaches.end(), "cargo") != loaded.disabledCaches.end());
    assert(std::find(loaded.ignoredCaches.begin(), loaded.ignoredCaches.end(), "npm") != loaded.ignoredCaches.end());
    assert(loaded.defaultSort == "size");
    assert(loaded.defaultCategory == "build");
    assert(loaded.customCaches.size() == 1);
    assert(loaded.customCaches[0].name == "test-cache");
    assert(loaded.customCaches[0].aliases[0] == "test");
    assert(loaded.customCaches[0].priority == 3);

    std::filesystem::remove_all(root);
    return 0;
}
