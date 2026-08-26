#include "core/StorageBudget.hpp"
#include <cassert>

int main()
{
    const StorageBudget budget = calculateStorageBudget(0, 4096, 2048);
    assert(budget.limitBytes == 0);
    assert(budget.currentBytes == 4096);
    assert(budget.projectedBytes == 4096);
    assert(budget.headroomBytes == 0);
    assert(!budget.exceeded);
    return 0;
}
