#include "core/CleanupPlanner.hpp"

#include <cassert>
#include <filesystem>

int main()
{
    ScanResult first;
    first.name = "safe-a";
    first.location = std::filesystem::path("/tmp/devclean-safe-a");
    first.found = true;
    first.bytes = 100;

    ScanResult second;
    second.name = "safe-b";
    second.location = std::filesystem::path("/tmp/devclean-safe-b");
    second.found = true;
    second.bytes = 50;

    const CleanupPlan plan = buildCleanupPlan({first, second}, 120, true);
    assert(plan.candidateCount == 2);
    assert(plan.plannedBytes >= 120);
    assert(plan.safeBytes == plan.plannedBytes);
    assert(plan.unsafeBytes == 0);
    assert(plan.targetReached);
    return 0;
}
