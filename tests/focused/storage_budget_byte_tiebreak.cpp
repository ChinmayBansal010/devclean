#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"small",100,50,true},{"large",200,50,true}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="large");return 0;}
