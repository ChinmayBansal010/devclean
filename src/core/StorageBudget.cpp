#include "core/StorageBudget.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

StorageBudget calculateStorageBudget(uint64_t limitBytes,
                                     uint64_t currentBytes,
                                     uint64_t reclaimableBytes)
{
    StorageBudget budget;
    budget.limitBytes = limitBytes;
    budget.currentBytes = currentBytes;
    budget.reclaimableBytes = std::min(reclaimableBytes, currentBytes);

    if (limitBytes == 0)
    {
        budget.projectedBytes = currentBytes;
        budget.headroomBytes = 0;
        budget.requiredReclaimBytes = 0;
        budget.canMeetLimit = true;
        budget.exceeded = false;
        budget.utilization = 0.0;
        budget.projectedUtilization = 0.0;
        return budget;
    }

    budget.projectedBytes = currentBytes > budget.reclaimableBytes ? currentBytes - budget.reclaimableBytes : 0;
    budget.exceeded = currentBytes > limitBytes;
    budget.headroomBytes = budget.projectedBytes < limitBytes ? limitBytes - budget.projectedBytes : 0;
    budget.requiredReclaimBytes = currentBytes > limitBytes ? currentBytes - limitBytes : 0;
    budget.canMeetLimit = budget.reclaimableBytes >= budget.requiredReclaimBytes;
    budget.utilization = static_cast<double>(currentBytes) / static_cast<double>(limitBytes);
    budget.projectedUtilization = static_cast<double>(budget.projectedBytes) / static_cast<double>(limitBytes);
    return budget;
}

std::vector<StorageBudgetItem> rankBudgetCandidates(std::vector<StorageBudgetItem> items)
{
    for (auto& item : items)
        item.reclaimableBytes = std::min(item.reclaimableBytes, item.bytes);
    std::stable_sort(items.begin(), items.end(), [](const StorageBudgetItem& lhs, const StorageBudgetItem& rhs) {
        if (lhs.safe != rhs.safe) return lhs.safe > rhs.safe;
        if (lhs.reclaimableBytes != rhs.reclaimableBytes) return lhs.reclaimableBytes > rhs.reclaimableBytes;
        if (lhs.bytes != rhs.bytes) return lhs.bytes > rhs.bytes;
        return lhs.name < rhs.name;
    });
    return items;
}

double budgetUsageRatio(const StorageBudget& budget)
{
    return budget.utilization;
}

double budgetProjectedUsageRatio(const StorageBudget& budget)
{
    return budget.projectedUtilization;
}

uint64_t budgetRequiredReclaim(const StorageBudget& budget)
{
    return budget.requiredReclaimBytes;
}

double budgetReclaimCoverage(const StorageBudget& budget)
{
    if (budget.requiredReclaimBytes == 0)
        return 1.0;
    return std::min(1.0, static_cast<double>(budget.reclaimableBytes) /
                           static_cast<double>(budget.requiredReclaimBytes));
}

double budgetHeadroomRatio(const StorageBudget& budget)
{
    if (budget.limitBytes == 0)
        return 0.0;
    return static_cast<double>(budget.headroomBytes) / static_cast<double>(budget.limitBytes);
}

uint64_t budgetSavingsBytes(const StorageBudget& budget)
{
    return budget.currentBytes > budget.projectedBytes ? budget.currentBytes - budget.projectedBytes : 0;
}

double budgetSavingsRatio(const StorageBudget& budget)
{
    if (budget.currentBytes == 0)
        return 0.0;
    return static_cast<double>(budgetSavingsBytes(budget)) / static_cast<double>(budget.currentBytes);
}

bool budgetWithinLimit(const StorageBudget& budget)
{
    return budget.limitBytes == 0 || budget.currentBytes <= budget.limitBytes;
}

bool budgetNeedsReclaim(const StorageBudget& budget)
{
    return budget.requiredReclaimBytes > 0;
}

bool budgetHasShortfall(const StorageBudget& budget)
{
    return budget.requiredReclaimBytes > budget.reclaimableBytes;
}

bool budgetCanMeetLimit(const StorageBudget& budget)
{
    return budget.canMeetLimit;
}

bool budgetHasHeadroom(const StorageBudget& budget)
{
    return budget.headroomBytes > 0;
}

bool budgetHasReclaimableData(const StorageBudget& budget)
{
    return budget.reclaimableBytes > 0;
}
