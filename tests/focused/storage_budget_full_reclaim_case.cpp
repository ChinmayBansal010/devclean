#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(4096, 4096, 4096);
    assert(budget.reclaimableBytes == 4096);
    assert(budget.projectedBytes == 0);
    assert(budget.headroomBytes == 4096);
    return 0;
}
