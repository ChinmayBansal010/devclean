#include "core/StorageBudget.hpp"

#include <cassert>

int main()
{
    const StorageBudget feasible = calculateStorageBudget(100, 160, 80);
    assert(feasible.exceeded);
    assert(feasible.requiredReclaimBytes == 60);
    assert(feasible.canMeetLimit);

    const StorageBudget infeasible = calculateStorageBudget(100, 160, 40);
    assert(infeasible.exceeded);
    assert(infeasible.requiredReclaimBytes == 60);
    assert(!infeasible.canMeetLimit);
    return 0;
}
