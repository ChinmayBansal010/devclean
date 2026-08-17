#include "engine/GrowthModel.hpp"

#include <algorithm>
#include <numeric>

GrowthProjection projectCacheGrowth(const std::vector<int64_t>& deltas)
{
    GrowthProjection projection;
    if (deltas.empty())
        return projection;

    projection.latestDelta = deltas.front();
    const int64_t sum = std::accumulate(deltas.begin(), deltas.end(), int64_t{0});
    projection.averageDelta = sum / static_cast<int64_t>(deltas.size());
    projection.dailyRate = static_cast<double>(projection.averageDelta);
    projection.projectedThirtyDays = projection.averageDelta * 30;

    if (deltas.size() >= 3)
    {
        const auto midpoint = deltas.size() / 2;
        const int64_t recent = std::accumulate(deltas.begin(), deltas.begin() + midpoint, int64_t{0});
        const int64_t older = std::accumulate(deltas.begin() + midpoint, deltas.end(), int64_t{0});
        projection.accelerating = recent > older;
    }
    return projection;
}
