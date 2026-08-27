#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct StorageBudget
{
    uint64_t limitBytes = 0;
    uint64_t currentBytes = 0;
    uint64_t reclaimableBytes = 0;
    uint64_t projectedBytes = 0;
    uint64_t headroomBytes = 0;
    uint64_t requiredReclaimBytes = 0;
    bool exceeded = false;
    bool canMeetLimit = true;
    double utilization = 0.0;
    double projectedUtilization = 0.0;
};

struct StorageBudgetItem
{
    std::string name;
    uint64_t bytes = 0;
    uint64_t reclaimableBytes = 0;
    bool safe = false;
};

StorageBudget calculateStorageBudget(uint64_t limitBytes,
                                     uint64_t currentBytes,
                                     uint64_t reclaimableBytes);

std::vector<StorageBudgetItem> rankBudgetCandidates(std::vector<StorageBudgetItem> items);

double budgetUsageRatio(const StorageBudget& budget);
double budgetProjectedUsageRatio(const StorageBudget& budget);
