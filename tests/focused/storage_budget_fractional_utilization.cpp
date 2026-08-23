#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(3,1,0);assert(b.utilization>0.3333);assert(b.utilization<0.3334);return 0;}
