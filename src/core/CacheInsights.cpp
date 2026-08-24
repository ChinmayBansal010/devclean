#include "core/CacheInsights.hpp"

#include "core/CleanupPolicy.hpp"
#include "platform/Filesystem.hpp"
#include "utils/Formatter.hpp"

#include <chrono>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {

constexpr uint64_t KB = 1024ULL;
constexpr uint64_t MB = KB * 1024ULL;
constexpr uint64_t GB = MB * 1024ULL;
constexpr uint64_t TB = GB * 1024ULL;

int penaltyForTotalBytes(uint64_t totalBytes)
{
    if (totalBytes >= 1ULL * TB) return 40;
    if (totalBytes >= 512ULL * GB) return 35;
    if (totalBytes >= 256ULL * GB) return 30;
    if (totalBytes >= 128ULL * GB) return 26;
    if (totalBytes >= 64ULL * GB) return 22;
    if (totalBytes >= 32ULL * GB) return 18;
    if (totalBytes >= 16ULL * GB) return 14;
    if (totalBytes >= 8ULL * GB) return 10;
    if (totalBytes >= 4ULL * GB) return 6;
    if (totalBytes >= 2ULL * GB) return 3;
    return 0;
}

int penaltyForGrowth(int64_t growthBytes)
{
    if (growthBytes <= 0) return 0;
    if (growthBytes >= static_cast<int64_t>(8ULL * GB)) return 20;
    if (growthBytes >= static_cast<int64_t>(4ULL * GB)) return 16;
    if (growthBytes >= static_cast<int64_t>(2ULL * GB)) return 12;
    if (growthBytes >= static_cast<int64_t>(1ULL * GB)) return 8;
    if (growthBytes >= static_cast<int64_t>(512ULL * MB)) return 5;
    if (growthBytes >= static_cast<int64_t>(256ULL * MB)) return 3;
    return 1;
}

int penaltyForAge(uint64_t ageDays)
{
    if (ageDays >= 365) return 5;
    if (ageDays >= 180) return 4;
    if (ageDays >= 90) return 3;
    if (ageDays >= 30) return 1;
    return 0;
}

std::string formatSignedBytes(int64_t bytes)
{
    const std::string sign = bytes >= 0 ? "+" : "-";
    const uint64_t magnitude = static_cast<uint64_t>(bytes >= 0 ? bytes : -bytes);
    return sign + Formatter::formatBytes(magnitude);
}

uint64_t ageDays(const ScanResult& result)
{
    if (result.age.count() <= 0)
        return 0;
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::hours>(result.age).count() / 24);
}

CacheRecommendation makeRecommendation(const ScanResult& result, int priority, std::string reason, std::string action, bool safe)
{
    CacheRecommendation recommendation;
    recommendation.name = result.name;
    recommendation.bytes = result.bytes;
    recommendation.growthBytes = result.growthBytes;
    recommendation.priority = priority;
    recommendation.reason = std::move(reason);
    recommendation.action = std::move(action);
    recommendation.safe = safe;
    return recommendation;
}

}

std::string cacheHealthLabel(int score)
{
    if (score >= 90) return "Excellent";
    if (score >= 75) return "Healthy";
    if (score >= 60) return "Watch";
    if (score >= 40) return "At risk";
    return "Critical";
}

CacheInsights buildCacheInsights(const std::vector<ScanResult>& results,
                                 const std::vector<ScanSnapshot>& history)
{
    CacheInsights insights;
    std::vector<CacheRecommendation> recommendations;

    for (const auto& result : results)
    {
        if (!result.isUsable())
            continue;

        insights.foundCount += 1;
        insights.totalBytes += result.bytes;
        insights.totalFiles += result.files;
        insights.totalDirectories += result.directories;
        insights.activeCount += result.active ? 1 : 0;
        insights.warningCount += result.warnings.size();
        insights.largestCaches.emplace_back(result.name, result.bytes);

        if (result.growthBytes != 0)
            insights.cacheGrowth.emplace_back(result.name, result.growthBytes);

        const uint64_t currentAgeDays = ageDays(result);
        const bool safe = isCleanupSafe(result);

        if (safe && result.bytes >= 1ULL * GB)
            recommendations.push_back(makeRecommendation(result, 100, "inactive cache without warnings above 1 GB", "Safe cleanup candidate", true));
        else if (result.growthBytes >= static_cast<int64_t>(512ULL * MB))
            recommendations.push_back(makeRecommendation(result, 95, "cache grew by " + formatSignedBytes(result.growthBytes) + " since the previous scan", "Inspect this cache before it grows further", safe));
        else if (currentAgeDays >= 90 && result.bytes >= 250ULL * MB)
            recommendations.push_back(makeRecommendation(result, 85, "cache is old and still consuming meaningful disk space", "Review for cleanup", safe));
        else if (result.hasWarnings())
            recommendations.push_back(makeRecommendation(result, 70, "tool warnings suggest the cache may still be active", "Use safe cleanup or wait for the tool to finish", false));
        else if (result.active && result.bytes >= 500ULL * MB)
            recommendations.push_back(makeRecommendation(result, 60, "large cache is associated with an active tool", "Defer cleanup until the tool is idle", false));
        else if (result.bytes >= 500ULL * MB)
            recommendations.push_back(makeRecommendation(result, 55, "cache is large enough to be worth a cleanup review", "Review cleanup options", safe));
        else if (currentAgeDays >= 30 && result.bytes >= 100ULL * MB)
            recommendations.push_back(makeRecommendation(result, 50, "cache is old enough to justify a review", "Consider clearing it if the tool is no longer needed", safe));
    }

    std::stable_sort(insights.largestCaches.begin(), insights.largestCaches.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.second != rhs.second) return lhs.second > rhs.second;
        return lhs.first < rhs.first;
    });

    std::stable_sort(insights.cacheGrowth.begin(), insights.cacheGrowth.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.second != rhs.second) return lhs.second > rhs.second;
        return lhs.first < rhs.first;
    });

    std::stable_sort(recommendations.begin(), recommendations.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.priority != rhs.priority) return lhs.priority > rhs.priority;
        if (lhs.bytes != rhs.bytes) return lhs.bytes > rhs.bytes;
        if (lhs.growthBytes != rhs.growthBytes) return lhs.growthBytes > rhs.growthBytes;
        return lhs.name < rhs.name;
    });

    insights.recommendations = std::move(recommendations);

    if (!history.empty())
    {
        const std::size_t limit = std::min<std::size_t>(history.size(), 6);
        insights.history.reserve(limit);
        int64_t totalDelta = 0;
        std::size_t deltaCount = 0;

        for (std::size_t i = 0; i < limit; ++i)
        {
            const auto& snapshot = history[i];
            const int64_t delta = (i + 1 < limit)
                ? static_cast<int64_t>(snapshot.totalBytes) - static_cast<int64_t>(history[i + 1].totalBytes)
                : 0;
            CacheTrendPoint point;
            point.label = snapshot.getTimestampString();
            point.totalBytes = snapshot.totalBytes;
            point.deltaBytes = delta;
            insights.history.push_back(std::move(point));
            if (i + 1 < limit)
            {
                totalDelta += delta;
                ++deltaCount;
            }
        }

        insights.latestGrowthBytes = insights.history.empty() ? 0 : insights.history.front().deltaBytes;
        insights.averageGrowthBytes = deltaCount == 0 ? 0 : totalDelta / static_cast<int64_t>(deltaCount);
        if (history.size() >= 2 && history[1].totalBytes > 0)
        {
            const int64_t latest = static_cast<int64_t>(history[0].totalBytes);
            const int64_t previous = static_cast<int64_t>(history[1].totalBytes);
            insights.latestGrowthPercent = static_cast<double>(latest - previous) * 100.0 / static_cast<double>(previous);
        }
    }

    const int64_t warningPenalty = std::min<int64_t>(20, static_cast<int64_t>(insights.warningCount) * 4);
    const int64_t activePenalty = std::min<int64_t>(10, static_cast<int64_t>(insights.activeCount) * 2);
    const int64_t totalPenalty = penaltyForTotalBytes(insights.totalBytes);
    const int64_t growthPenalty = penaltyForGrowth(insights.latestGrowthBytes);
    int64_t agePenalty = 0;
    for (const auto& result : results)
        if (result.isUsable()) agePenalty += penaltyForAge(ageDays(result));
    agePenalty = std::min<int64_t>(15, agePenalty);

    int64_t score = 100 - warningPenalty - activePenalty - totalPenalty - growthPenalty - agePenalty;
    if (insights.foundCount == 0) score = 100;
    else if (insights.latestGrowthBytes < 0) score = std::min<int64_t>(100, score + 4);
    score = std::clamp<int64_t>(score, 0, 100);
    insights.health.score = static_cast<int>(score);
    insights.health.label = cacheHealthLabel(insights.health.score);

    if (insights.foundCount == 0)
        insights.health.factors.push_back("no cache directories were found");
    else
    {
        if (totalPenalty > 0) insights.health.factors.push_back("disk usage pressure: -" + std::to_string(totalPenalty));
        if (growthPenalty > 0) insights.health.factors.push_back("growth pressure: -" + std::to_string(growthPenalty));
        if (warningPenalty > 0) insights.health.factors.push_back("warnings: -" + std::to_string(warningPenalty));
        if (activePenalty > 0) insights.health.factors.push_back("active caches: -" + std::to_string(activePenalty));
        if (agePenalty > 0) insights.health.factors.push_back("age pressure: -" + std::to_string(agePenalty));
        if (insights.latestGrowthBytes < 0) insights.health.factors.push_back("recent growth is negative, which improves the score");
    }

    return insights;
}
