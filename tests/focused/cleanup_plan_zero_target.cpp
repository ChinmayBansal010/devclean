#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=40;ScanResult b;b.name="b";b.found=true;b.bytes=60;const auto p=buildCleanupPlan({a,b},0,false);assert(p.items.size()==2);assert(p.plannedBytes==100);assert(p.targetReached);return 0;}
