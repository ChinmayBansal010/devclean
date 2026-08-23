#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(0,4096,2048);assert(b.limitBytes==0);assert(b.currentBytes==4096);assert(b.projectedBytes==4096);assert(!b.exceeded);assert(b.utilization==0.0);return 0;}
