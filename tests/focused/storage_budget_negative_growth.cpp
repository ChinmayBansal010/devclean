#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,400,600);assert(b.projectedBytes==0);assert(b.headroomBytes==1000);return 0;}
