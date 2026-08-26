#include "utils/StringUtils.hpp"
#include <cassert>
int main()
{
    assert(StringUtils::trim("   cache") == "cache");
    return 0;
}
