#pragma once

#include <cstdint>
#include <vector>

struct GrowthProjection
{
    int64_t latestDelta = 0;
    int64_t averageDelta = 0;
    int64_t projectedThirtyDays = 0;
    double dailyRate = 0.0;
    bool accelerating = false;
};

GrowthProjection projectCacheGrowth(const std::vector<int64_t>& deltas);
