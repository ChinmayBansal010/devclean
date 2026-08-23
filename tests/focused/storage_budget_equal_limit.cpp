#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,100,25);assert(!b.exceeded);assert(b.projectedBytes==75);assert(b.utilization==1.0);return 0;}
