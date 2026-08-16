#include "scanner/ScannerEngine.hpp"

#include <cassert>
#include <cstdlib>

int main()
{
    const char* home = std::getenv("HOME");
    if (home == nullptr)
        return 0;

    assert(home[0] != '\0');
    return 0;
}
