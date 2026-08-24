#include "core/CleanupPolicy.hpp"

#include "platform/Filesystem.hpp"

#include <algorithm>

bool isCleanupSafe(const ScanResult& result)
{
    return result.found && !result.active && result.warnings.empty() &&
           !Filesystem::isProtectedPath(result.location);
}

int cleanupPriority(const ScanResult& result, bool safe)
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

bool cleanupCandidateMatches(const ScanResult& result, bool safeOnly)
{
    if (!result.found || result.bytes == 0)
        return false;
    return !safeOnly || isCleanupSafe(result);
}

uint64_t cleanupEffectiveBytes(const ScanResult& result)
{
    if (!result.found)
        return 0;
    return result.bytes;
}
