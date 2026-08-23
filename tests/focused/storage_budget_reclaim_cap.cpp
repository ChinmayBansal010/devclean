#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,50,90);assert(b.reclaimableBytes==50);assert(b.projectedBytes==0);return 0;}
