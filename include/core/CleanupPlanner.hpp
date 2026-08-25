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
    uint64_t safeBytes = 0;
    uint64_t unsafeBytes = 0;
    std::size_t candidateCount = 0;
    bool targetReached = false;
    std::vector<CleanupPlanItem> items;

    uint64_t shortfallBytes() const
    {
        return plannedBytes < requestedBytes ? requestedBytes - plannedBytes : 0;
    }

    bool hasShortfall() const
    {
        return shortfallBytes() != 0;
    }
};

CleanupPlan buildCleanupPlan(const std::vector<ScanResult>& results,
                             uint64_t targetBytes,
                             bool safeOnly);
