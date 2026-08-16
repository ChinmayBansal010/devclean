#include "core/CacheInsights.hpp"

#include <cassert>
#include <chrono>
#include <filesystem>

int main()
{
    const auto now = std::chrono::system_clock::now();

    ScanResult cargo;
    cargo.name = "cargo";
    cargo.bytes = 3ULL * 1024ULL * 1024ULL * 1024ULL;
    cargo.files = 10;
    cargo.directories = 2;
    cargo.found = true;
    cargo.active = false;
    cargo.location = std::filesystem::temp_directory_path() / "devclean-cache-insights" / "cargo";
    cargo.age = std::chrono::hours(24 * 120);
    cargo.growthBytes = 512LL * 1024LL * 1024LL;

    ScanResult npm;
    npm.name = "npm";
    npm.bytes = 250ULL * 1024ULL * 1024ULL;
    npm.files = 4;
    npm.directories = 1;
    npm.found = true;
    npm.active = true;
    npm.warnings = {"tool appears active"};
    npm.location = std::filesystem::temp_directory_path() / "devclean-cache-insights" / "npm";
    npm.age = std::chrono::hours(24 * 15);

    ScanSnapshot latest;
    latest.timestamp = now;
    latest.results = {cargo, npm};
    latest.totalBytes = cargo.bytes + npm.bytes;
    latest.totalFiles = cargo.files + npm.files;
    latest.totalDirectories = cargo.directories + npm.directories;

    ScanSnapshot previous;
    previous.timestamp = now - std::chrono::hours(24);
    previous.totalBytes = 2ULL * 1024ULL * 1024ULL * 1024ULL;
    previous.totalFiles = 12;
    previous.totalDirectories = 4;

    const CacheInsights insights = buildCacheInsights({cargo, npm}, {latest, previous});

    assert(insights.foundCount == 2);
    assert(insights.totalFiles == 14);
    assert(insights.totalDirectories == 3);
    assert(insights.totalBytes == cargo.bytes + npm.bytes);
    assert(insights.latestGrowthBytes == static_cast<int64_t>(latest.totalBytes) - static_cast<int64_t>(previous.totalBytes));
    assert(insights.averageGrowthBytes == insights.latestGrowthBytes);
    assert(insights.health.score >= 0);
    assert(insights.health.score <= 100);
    assert(!insights.health.label.empty());
    assert(!insights.history.empty());
    assert(!insights.recommendations.empty());
    assert(insights.recommendations.front().name == "cargo");
    assert(insights.recommendations.front().priority >= insights.recommendations.back().priority);

    return 0;
}
