#include "scanner/ScanResult.hpp"

#include <cassert>

int main()
{
    ScanResult result;
    result.name = "bazel";
    result.bytes = 1024;
    result.files = 4;
    result.directories = 2;
    result.found = true;
    result.active = true;

    assert(result.name == "bazel");
    assert(result.bytes == 1024);
    assert(result.files == 4);
    assert(result.directories == 2);
    assert(result.found);
    assert(result.active);
    return 0;
}
