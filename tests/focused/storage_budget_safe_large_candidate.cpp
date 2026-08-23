#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"safe",1ULL<<40,1ULL<<39,true},{"unsafe",1ULL<<50,1ULL<<50,false}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="safe");return 0;}
