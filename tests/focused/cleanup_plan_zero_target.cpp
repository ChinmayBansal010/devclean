#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){const std::vector<ScanResult> results;const auto p=buildCleanupPlan(results,0,false);assert(p.targetReached);assert(p.plannedBytes==0);return 0;}
