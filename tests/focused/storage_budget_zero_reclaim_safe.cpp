#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"safe",100,0,true},{"unsafe",100,0,false}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="safe");return 0;}
