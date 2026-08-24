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
        budget.exceeded = false;
        budget.utilization = 0.0;
        return budget;
    }

    budget.projectedBytes = currentBytes > budget.reclaimableBytes
        ? currentBytes - budget.reclaimableBytes
        : 0;
    budget.exceeded = currentBytes > limitBytes;
    budget.utilization = static_cast<double>(currentBytes) /
                         static_cast<double>(limitBytes);
    return budget;
}

std::vector<StorageBudgetItem> rankBudgetCandidates(std::vector<StorageBudgetItem> items)
{
    for (auto& item : items)
        item.reclaimableBytes = std::min(item.reclaimableBytes, item.bytes);

    std::stable_sort(items.begin(), items.end(), [](const StorageBudgetItem& lhs,
                                                     const StorageBudgetItem& rhs) {
        if (lhs.safe != rhs.safe)
            return lhs.safe > rhs.safe;
        if (lhs.reclaimableBytes != rhs.reclaimableBytes)
            return lhs.reclaimableBytes > rhs.reclaimableBytes;
        if (lhs.bytes != rhs.bytes)
            return lhs.bytes > rhs.bytes;
        return lhs.name < rhs.name;
    });
    return items;
}
