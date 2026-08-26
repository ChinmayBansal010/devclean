#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,500,0);assert(b.utilization==0.5);assert(b.projectedUtilization==0.5);return 0;}
