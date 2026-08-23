#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,0,50);assert(b.currentBytes==0);assert(b.reclaimableBytes==0);assert(b.projectedBytes==0);assert(b.utilization==0.0);return 0;}
