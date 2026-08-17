#include "engine/CachePolicy.hpp"

#include <algorithm>

CachePolicyDecision evaluateCachePolicy(const CachePolicyInput& input)
{
    CachePolicyDecision decision;
    decision.safe = !input.active && !input.warned && !input.protectedPath;

    if (input.protectedPath)
    {
        decision.action = "protect";
        decision.reason = "path is protected";
        decision.priority = 100;
        decision.safe = false;
        return decision;
    }

    if (input.active || input.warned)
    {
        decision.action = "defer";
        decision.reason = input.active ? "associated tool is active" : "cache has warnings";
        decision.priority = 80;
        decision.safe = false;
        return decision;
    }

    if (input.bytes >= 1024ULL * 1024ULL * 1024ULL)
        decision.priority += 30;
    else if (input.bytes >= 256ULL * 1024ULL * 1024ULL)
        decision.priority += 15;

    if (input.growthBytes > 0)
        decision.priority += input.growthBytes >= static_cast<int64_t>(1024ULL * 1024ULL * 1024ULL) ? 20 : 10;

    if (input.ageSeconds >= 90ULL * 24ULL * 60ULL * 60ULL)
        decision.priority += 15;

    decision.priority = std::clamp(decision.priority, 0, 100);
    decision.action = decision.priority >= 50 ? "review" : "keep";
    decision.reason = decision.action == "review" ? "inactive cache has meaningful cleanup value" : "cache does not meet cleanup priority";
    return decision;
}
