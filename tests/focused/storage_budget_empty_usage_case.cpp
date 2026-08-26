#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(1024, 0, 0);
    assert(budget.currentBytes == 0);
    assert(budget.projectedBytes == 0);
    assert(!budget.exceeded);
    return 0;
}
