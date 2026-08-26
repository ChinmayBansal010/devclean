#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(1024, 2048, 0);
    assert(budget.reclaimableBytes == 0);
    assert(budget.projectedBytes == 2048);
    assert(budget.exceeded);
    return 0;
}
