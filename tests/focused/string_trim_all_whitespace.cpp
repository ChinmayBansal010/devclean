#include "utils/StringUtils.hpp"
#include <cassert>
int main()
{
    assert(StringUtils::trim(" \t\n") == "");
    return 0;
}
