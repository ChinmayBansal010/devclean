#include "scanner/ScannerEngine.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

int main()
{
    const auto root = std::filesystem::temp_directory_path() / "devclean-scanner-test";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "cache" / "nested");
    std::ofstream(root / "cache" / "artifact.bin") << std::string(1024, 'x');
    std::ofstream(root / "cache" / "nested" / "source.txt") << "devclean";

    AppConfig config;
    CacheDefinition cache;
    cache.name = "test-cache";
    cache.category = "test";
    cache.enabled = true;
    cache.linuxPath = root / "cache";
    cache.windowsPath = root / "cache";
    cache.cachePaths = {root / "cache"};
    config.customCaches.push_back(cache);

    ScannerEngine scanner;
    const auto results = scanner.scan({"test-cache"}, config);

    assert(results.size() == 1);
    assert(results[0].name == "test-cache");
    assert(results[0].found);
    assert(results[0].bytes > 0);
    assert(results[0].files == 2);
    assert(results[0].directories >= 1);
    assert(results[0].location == root / "cache");

    std::filesystem::remove_all(root);
    return 0;
}
