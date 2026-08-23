#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="a";a.found=true;a.bytes=30;ScanResult b;b.name="b";b.found=true;b.bytes=20;const auto p=buildCleanupPlan({a,b},45,false);assert(p.items.size()==2);assert(p.plannedBytes==50);assert(p.targetReached);return 0;}
