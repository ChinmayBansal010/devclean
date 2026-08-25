#include "core/CleanupPlanner.hpp"

#include <cassert>
#include <filesystem>

int main()
{
    ScanResult result;
    result.name = "small-cache";
    result.location = std::filesystem::path("/tmp/devclean-small");
    result.found = true;
    result.bytes = 40;

    const CleanupPlan plan = buildCleanupPlan({result}, 100, true);
    assert(plan.plannedBytes == 40);
    assert(plan.shortfallBytes() == 60);
    assert(plan.hasShortfall());
    return 0;
}
