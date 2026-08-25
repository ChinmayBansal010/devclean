#include "scanner/ScanResult.hpp"

#include <cassert>
#include <chrono>

int main()
{
    ScanResult result;
    result.found = true;
    result.growthBytes = 128;
    result.age = std::chrono::hours(48);

    assert(result.isUsable());
    assert(result.hasGrowth());
    assert(result.isStale(std::chrono::hours(24)));
    assert(!result.isStale(std::chrono::hours(72)));

    result.skipped = true;
    assert(!result.isUsable());
    return 0;
}
