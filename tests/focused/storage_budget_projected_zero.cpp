#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(10,5,100);assert(b.projectedBytes==0);assert(b.reclaimableBytes==5);return 0;}
