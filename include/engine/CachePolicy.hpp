#pragma once

#include <cstdint>
#include <string>

struct CachePolicyInput
{
    uint64_t bytes = 0;
    int64_t growthBytes = 0;
    uint64_t ageSeconds = 0;
    bool active = false;
    bool warned = false;
    bool protectedPath = false;
};

struct CachePolicyDecision
{
    std::string action;
    std::string reason;
    int priority = 0;
    bool safe = false;
};

CachePolicyDecision evaluateCachePolicy(const CachePolicyInput& input);
