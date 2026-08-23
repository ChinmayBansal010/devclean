#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"small",100,10,true},{"large",100,90,true}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="large");return 0;}
