#include "core/CacheInsights.hpp"
#include <cassert>
#include <chrono>
#include <filesystem>
int main()
{
    const auto now = std::chrono::system_clock::now();
    ScanResult cargo;
    cargo.name = "cargo"; cargo.bytes = 3ULL * 1024 * 1024 * 1024; cargo.files = 10; cargo.directories = 2; cargo.found = true; cargo.active = false;
    cargo.location = std::filesystem::temp_directory_path() / "devclean-cache-insights" / "cargo"; cargo.age = std::chrono::hours(24 * 120); cargo.growthBytes = 512LL * 1024 * 1024;
    ScanResult npm;
    npm.name = "npm"; npm.bytes = 250ULL * 1024 * 1024; npm.files = 4; npm.directories = 1; npm.found = true; npm.active = true; npm.warnings = {"tool appears active"};
    npm.location = std::filesystem::temp_directory_path() / "devclean-cache-insights" / "npm"; npm.age = std::chrono::hours(24 * 15);
    ScanSnapshot latest; latest.timestamp = now; latest.results = {cargo, npm}; latest.totalBytes = cargo.bytes + npm.bytes; latest.totalFiles = 14; latest.totalDirectories = 3;
    ScanSnapshot previous; previous.timestamp = now - std::chrono::hours(24); previous.totalBytes = 2ULL * 1024 * 1024 * 1024; previous.totalFiles = 12; previous.totalDirectories = 4;
    const CacheInsights insights = buildCacheInsights({cargo, npm}, {latest, previous});
    assert(insights.foundCount == 2); assert(insights.totalFiles == 14); assert(insights.totalDirectories == 3); assert(insights.totalBytes == cargo.bytes + npm.bytes);
    assert(insights.latestGrowthBytes == static_cast<int64_t>(latest.totalBytes) - static_cast<int64_t>(previous.totalBytes)); assert(insights.averageGrowthBytes == insights.latestGrowthBytes);
    assert(insights.health.score >= 0); assert(insights.health.score <= 100); assert(!insights.health.label.empty()); assert(!insights.history.empty()); assert(!insights.recommendations.empty());
    assert(insights.recommendations.front().name == "cargo"); assert(insights.recommendations.front().priority >= insights.recommendations.back().priority);
    assert(cacheHealthLabel(100) == "Excellent"); assert(cacheHealthLabel(89) == "Healthy"); assert(cacheHealthLabel(74) == "Watch"); assert(cacheHealthLabel(59) == "At risk"); assert(cacheHealthLabel(39) == "Critical");
    assert(insights.recommendations.front().safe); assert(!insights.recommendations.back().safe); assert(insights.largestCaches.front().first == "cargo"); assert(insights.cacheGrowth.front().first == "cargo");
    assert(insights.history.front().totalBytes == latest.totalBytes);
    return 0;
}
