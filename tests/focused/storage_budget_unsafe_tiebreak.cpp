#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"unsafe-small",100,90,false},{"safe-small",100,80,true}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="safe-small");return 0;}
