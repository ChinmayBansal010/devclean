#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"unsafe",500,500,false},{"safe",100,100,true}};const auto ranked=rankBudgetCandidates(items);assert(ranked.front().name=="safe");assert(ranked.back().name=="unsafe");return 0;}
