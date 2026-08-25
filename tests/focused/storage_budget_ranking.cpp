#include "core/StorageBudget.hpp"

#include <cassert>
#include <vector>

int main()
{
    const auto ranked = rankBudgetCandidates({
        {"unsafe-large", 900, 900, false},
        {"safe-small", 200, 200, true},
        {"safe-large", 700, 700, true}
    });

    assert(ranked.size() == 3);
    assert(ranked[0].name == "safe-large");
    assert(ranked[1].name == "safe-small");
    assert(ranked[2].name == "unsafe-large");
    return 0;
}
