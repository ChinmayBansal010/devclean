#include "scanner/ScanResult.hpp"
#include <cassert>
int main(){ScanResult r;r.found=true;r.enabled=true;r.skipped=false;assert(r.isUsable());return 0;}
