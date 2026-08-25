#include "core/StorageBudget.hpp"

#include <cassert>

int main()
{
    const StorageBudget budget = calculateStorageBudget(100, 50, 200);
    assert(budget.reclaimableBytes == 50);
    assert(budget.projectedBytes == 0);
    assert(budget.canMeetLimit);
    return 0;
}
