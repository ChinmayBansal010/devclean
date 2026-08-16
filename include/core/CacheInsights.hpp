#pragma once

#include "core/ScanHistory.hpp"
#include "scanner/ScanResult.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

struct CacheHealthSummary
{
    int score = 100;
    std::string label;
    std::vector<std::string> factors;
};

struct CacheTrendPoint
{
    std::string label;
    uint64_t totalBytes = 0;
    int64_t deltaBytes = 0;
};

struct CacheRecommendation
{
    std::string name;
    std::string reason;
    std::string action;
    uint64_t bytes = 0;
    int64_t growthBytes = 0;
    int priority = 0;
    bool safe = false;
};

struct CacheInsights
{
    CacheHealthSummary health;
    uint64_t totalBytes = 0;
    uint64_t totalFiles = 0;
    uint64_t totalDirectories = 0;
    std::size_t foundCount = 0;
    std::size_t activeCount = 0;
    std::size_t warningCount = 0;
    int64_t latestGrowthBytes = 0;
    int64_t averageGrowthBytes = 0;
    double latestGrowthPercent = 0.0;
    std::vector<CacheTrendPoint> history;
    std::vector<std::pair<std::string, int64_t>> cacheGrowth;
    std::vector<CacheRecommendation> recommendations;
    std::vector<std::pair<std::string, uint64_t>> largestCaches;
};

CacheInsights buildCacheInsights(const std::vector<ScanResult>& results,
                                 const std::vector<ScanSnapshot>& history);

std::string cacheHealthLabel(int score);
