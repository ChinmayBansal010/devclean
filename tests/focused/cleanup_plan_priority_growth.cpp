#include "core/CleanupPlanner.hpp"
#include <cassert>
int main(){ScanResult a;a.name="growth";a.found=true;a.bytes=100;a.growthBytes=10;ScanResult b;b.name="plain";b.found=true;b.bytes=100;const auto p=buildCleanupPlan({a,b},0,false);assert(p.items.front().name=="growth");assert(p.items.front().priority>p.items.back().priority);return 0;}
