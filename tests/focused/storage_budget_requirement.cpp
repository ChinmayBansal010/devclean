#include "core/StorageBudget.hpp"

#include <cassert>

int main()
{
    const StorageBudget exceeded = calculateStorageBudget(100, 160, 20);
    assert(exceeded.exceeded);
    assert(exceeded.requiredReclaimBytes == 60);

    const StorageBudget within = calculateStorageBudget(200, 160, 20);
    assert(!within.exceeded);
    assert(within.requiredReclaimBytes == 0);

    return 0;
}
