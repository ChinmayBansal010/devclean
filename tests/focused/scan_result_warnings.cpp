#include "scanner/ScanResult.hpp"

#include <cassert>

int main()
{
    ScanResult result;
    assert(!result.hasWarnings());

    result.warnings.push_back("tool is running");
    assert(result.hasWarnings());
    assert(result.isInactive());

    result.active = true;
    assert(!result.isInactive());
    return 0;
}
