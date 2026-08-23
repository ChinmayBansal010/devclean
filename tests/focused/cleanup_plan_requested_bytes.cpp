#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){const auto p=buildCleanupPlan({},4096,false);assert(p.requestedBytes==4096);assert(!p.targetReached);return 0;}
