#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,900,250);assert(b.projectedBytes==650);assert(b.projectedUtilization==0.65);return 0;}
