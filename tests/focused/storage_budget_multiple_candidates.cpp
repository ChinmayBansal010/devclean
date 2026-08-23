#include "core/StorageBudget.hpp"
#include <cassert>
int main(){std::vector<StorageBudgetItem> items={{"mid",200,100,true},{"top",300,150,true},{"low",500,10,true}};const auto ranked=rankBudgetCandidates(items);assert(ranked[0].name=="top");assert(ranked[1].name=="mid");assert(ranked[2].name=="low");return 0;}
