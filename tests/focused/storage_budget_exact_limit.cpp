#include "core/StorageBudget.hpp"

int main()
{
    const auto budget = calculateStorageBudget(1000, 1000, 0);

    if (budget.exceeded)
        return 1;

    if (budget.headroomBytes != 0)
        return 2;

    if (budget.requiredReclaimBytes != 0)
        return 3;

    if (!budget.canMeetLimit)
        return 4;

    return 0;
}
