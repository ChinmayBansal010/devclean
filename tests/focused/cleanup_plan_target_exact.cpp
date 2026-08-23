#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=100;ScanResult b;b.name="b";b.found=true;b.bytes=200;const auto p=buildCleanupPlan({a,b},100,false);assert(p.plannedBytes==100);assert(p.targetReached);assert(p.items.size()==1);return 0;}
