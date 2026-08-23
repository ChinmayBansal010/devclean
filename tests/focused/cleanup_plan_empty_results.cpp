#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){const auto p=buildCleanupPlan({},0,false);assert(p.requestedBytes==0);assert(p.plannedBytes==0);assert(p.targetReached);assert(p.items.empty());return 0;}
