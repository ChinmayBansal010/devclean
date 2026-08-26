#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(0, 4096, 1024);
    assert(budget.limitBytes == 0);
    assert(budget.projectedBytes == 4096);
    assert(budget.headroomBytes == 0);
    assert(!budget.exceeded);
    return 0;
}
