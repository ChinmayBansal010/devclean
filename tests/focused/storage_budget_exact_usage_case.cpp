#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(4096, 4096, 0);
    assert(budget.exceeded == false);
    assert(budget.headroomBytes == 0);
    assert(budget.utilization == 1.0);
    return 0;
}
