#include "core/CleanupPlanner.hpp"

#include "core/CleanupPolicy.hpp"

#include <algorithm>
#include <limits>

namespace {

std::size_t bestCandidate(const std::vector<CleanupPlanItem>& candidates,
                          const std::vector<bool>& selected,
                          uint64_t remaining)
{
    std::size_t best = candidates.size();
    uint64_t bestBytes = 0;

    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        if (selected[i])
            continue;
        const uint64_t bytes = candidates[i].bytes;
        if (bytes <= remaining && bytes >= bestBytes)
        {
            best = i;
            bestBytes = bytes;
        }
    }

    if (best != candidates.size())
        return best;

    uint64_t smallestOvershoot = std::numeric_limits<uint64_t>::max();
    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        if (selected[i] || candidates[i].bytes < remaining)
            continue;
        if (candidates[i].bytes < smallestOvershoot)
        {
            smallestOvershoot = candidates[i].bytes;
            best = i;
        }
    }

    return best;
}

void accountPlanItem(CleanupPlan& plan, const CleanupPlanItem& item)
{
    plan.plannedBytes += item.bytes;
    if (item.safe)
        plan.safeBytes += item.bytes;
    else
        plan.unsafeBytes += item.bytes;
}

}

CleanupPlan buildCleanupPlan(const std::vector<ScanResult>& results,
                             uint64_t targetBytes,
                             bool safeOnly)
{
    CleanupPlan plan;
    plan.requestedBytes = targetBytes;

    std::vector<CleanupPlanItem> candidates;
    for (const auto& result : results)
    {
        if (!cleanupCandidateMatches(result, safeOnly))
            continue;

        const bool safe = isCleanupSafe(result);
        CleanupPlanItem item;
        item.name = result.name;
        item.bytes = cleanupEffectiveBytes(result);
        item.safe = safe;
        item.priority = cleanupPriority(result, safe);
        item.reason = safe ? "inactive and unprotected" : "requires explicit review";
        candidates.push_back(std::move(item));
    }

    plan.candidateCount = candidates.size();

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
            accountPlanItem(plan, item);
        plan.targetReached = true;
        return plan;
    }

    std::vector<bool> selected(candidates.size(), false);
    while (plan.plannedBytes < targetBytes)
    {
        const uint64_t remaining = targetBytes - plan.plannedBytes;
        const std::size_t index = bestCandidate(candidates, selected, remaining);
        if (index == candidates.size())
            break;

        selected[index] = true;
        plan.items.push_back(candidates[index]);
        accountPlanItem(plan, candidates[index]);
    }

    plan.targetReached = plan.plannedBytes >= targetBytes;
    return plan;
}
