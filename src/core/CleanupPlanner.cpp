#include "core/CleanupPlanner.hpp"

#include "platform/Filesystem.hpp"

#include <algorithm>

namespace {

bool isSafe(const ScanResult& result)
{
    return result.found && !result.active && result.warnings.empty() &&
           !Filesystem::isProtectedPath(result.location);
}

int priorityFor(const ScanResult& result, bool safe)
{
    int priority = safe ? 100 : 40;
    if (result.growthBytes > 0)
        priority += 20;
    if (result.bytes >= 1024ULL * 1024ULL * 1024ULL)
        priority += 10;
    if (result.active)
        priority -= 30;
    if (!result.warnings.empty())
        priority -= 20;
    return std::max(0, priority);
}

} // namespace

CleanupPlan buildCleanupPlan(const std::vector<ScanResult>& results,
                             uint64_t targetBytes,
                             bool safeOnly)
{
    CleanupPlan plan;
    plan.requestedBytes = targetBytes;

    std::vector<CleanupPlanItem> candidates;
    for (const auto& result : results)
    {
        if (!result.found || result.bytes == 0)
            continue;

        const bool safe = isSafe(result);
        if (safeOnly && !safe)
            continue;

        CleanupPlanItem item;
        item.name = result.name;
        item.bytes = result.bytes;
        item.safe = safe;
        item.priority = priorityFor(result, safe);
        item.reason = safe ? "inactive and unprotected" : "requires explicit review";
        candidates.push_back(std::move(item));
    }

    std::stable_sort(candidates.begin(), candidates.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.priority != rhs.priority)
            return lhs.priority > rhs.priority;
        if (lhs.bytes != rhs.bytes)
            return lhs.bytes > rhs.bytes;
        return lhs.name < rhs.name;
    });

    if (targetBytes == 0)
    {
        plan.items = std::move(candidates);
        for (const auto& item : plan.items)
            plan.plannedBytes += item.bytes;
        plan.targetReached = true;
        return plan;
    }

    for (const auto& item : candidates)
    {
        if (plan.plannedBytes >= targetBytes)
            break;
        plan.items.push_back(item);
        plan.plannedBytes += item.bytes;
    }

    plan.targetReached = plan.plannedBytes >= targetBytes;
    return plan;
}
