#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,800,100);assert(b.projectedBytes==700);assert(b.headroomBytes==300);return 0;}
