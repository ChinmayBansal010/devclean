#pragma once

#include "scanner/ScanResult.hpp"

#include <cstdint>
#include <string>
#include <vector>

struct CleanupPlanItem
{
    std::string name;
    std::string reason;
    uint64_t bytes = 0;
    int priority = 0;
    bool safe = false;
};

struct CleanupPlan
{
    uint64_t requestedBytes = 0;
    uint64_t plannedBytes = 0;
    bool targetReached = false;
    std::vector<CleanupPlanItem> items;
};

CleanupPlan buildCleanupPlan(const std::vector<ScanResult>& results,
                             uint64_t targetBytes,
                             bool safeOnly);
