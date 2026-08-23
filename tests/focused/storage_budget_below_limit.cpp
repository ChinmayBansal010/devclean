#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,40,10);assert(!b.exceeded);assert(b.projectedBytes==30);assert(b.utilization==0.4);return 0;}
