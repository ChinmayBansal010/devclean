#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,140,20);assert(b.exceeded);assert(b.projectedBytes==120);assert(b.utilization==1.4);return 0;}
