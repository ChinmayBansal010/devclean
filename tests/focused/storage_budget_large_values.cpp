#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1ULL<<40,1ULL<<39,1ULL<<38);assert(!b.exceeded);assert(b.projectedBytes==(1ULL<<38));assert(b.utilization==0.5);return 0;}
