#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=150;const auto p=buildCleanupPlan({a},100,false);assert(p.plannedBytes==150);assert(p.targetReached);return 0;}
