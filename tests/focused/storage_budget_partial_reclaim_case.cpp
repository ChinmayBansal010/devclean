#include "core/StorageBudget.hpp"
#include <cassert>
int main()
{
    const auto budget = calculateStorageBudget(4096, 4096, 1024);
    assert(budget.reclaimableBytes == 1024);
    assert(budget.projectedBytes == 3072);
    assert(budget.headroomBytes == 1024);
    return 0;
}
