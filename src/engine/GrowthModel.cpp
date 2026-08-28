#include "engine/GrowthModel.hpp"

#include <algorithm>
#include <limits>
#include <numeric>

namespace {

int64_t saturatingAdd(int64_t lhs, int64_t rhs)
{
    if (rhs > 0 && lhs > std::numeric_limits<int64_t>::max() - rhs)
        return std::numeric_limits<int64_t>::max();
    if (rhs < 0 && lhs < std::numeric_limits<int64_t>::min() - rhs)
        return std::numeric_limits<int64_t>::min();
    return lhs + rhs;
}

int64_t saturatingMultiply(int64_t value, int64_t factor)
{
    if (value > 0 && factor > 0 && value > std::numeric_limits<int64_t>::max() / factor)
        return std::numeric_limits<int64_t>::max();
    if (value < 0 && factor > 0 && value < std::numeric_limits<int64_t>::min() / factor)
        return std::numeric_limits<int64_t>::min();
    return value * factor;
}

} // namespace

GrowthProjection projectCacheGrowth(const std::vector<int64_t>& deltas)
{
    GrowthProjection projection;
    if (deltas.empty())
        return projection;

    projection.latestDelta = deltas.front();
    const int64_t sum = std::accumulate(deltas.begin(), deltas.end(), int64_t{0}, saturatingAdd);
    projection.averageDelta = sum / static_cast<int64_t>(deltas.size());
    projection.dailyRate = static_cast<double>(projection.averageDelta);
    projection.projectedThirtyDays = saturatingMultiply(projection.averageDelta, 30);

    if (deltas.size() >= 3)
    {
        const auto midpoint = deltas.size() / 2;
        const int64_t recent = std::accumulate(deltas.begin(), deltas.begin() + midpoint, int64_t{0}, saturatingAdd);
        const int64_t older = std::accumulate(deltas.begin() + midpoint, deltas.end(), int64_t{0}, saturatingAdd);
        projection.accelerating = recent > older;
    }
    return projection;
}
