#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(4096, 2048, 0);
    assert(!budget.exceeded);
    assert(budget.headroomBytes == 2048);
    assert(budget.projectedBytes == 2048);
    return 0;
}
