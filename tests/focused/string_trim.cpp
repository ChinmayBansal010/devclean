#include "utils/StringUtils.hpp"

#include <cassert>

int main()
{
    assert(StringUtils::trim("  devclean  ") == "devclean");
    assert(StringUtils::trim("\tpython\n") == "python");
    assert(StringUtils::trim("   ").empty());
    return 0;
}
