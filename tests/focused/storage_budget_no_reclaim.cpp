#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,80,0);assert(b.reclaimableBytes==0);assert(b.projectedBytes==80);assert(!b.exceeded);return 0;}
