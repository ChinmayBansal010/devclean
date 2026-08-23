#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"zeta",100,50,true},{"alpha",100,50,true}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="alpha");return 0;}
