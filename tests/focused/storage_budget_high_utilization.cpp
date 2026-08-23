#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,900,100);assert(b.utilization==0.9);assert(!b.exceeded);assert(b.projectedBytes==800);return 0;}
