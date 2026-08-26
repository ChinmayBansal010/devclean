#include "core/StorageBudget.hpp"
#include <cassert>
int main(){const auto b=calculateStorageBudget(1000,1000,0);assert(b.exceeded==false);assert(b.headroomBytes==1000);return 0;}
