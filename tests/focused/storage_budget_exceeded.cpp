#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(100,150,20);assert(b.exceeded);assert(b.projectedBytes==130);return 0;}
