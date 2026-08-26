#include "utils/StringUtils.hpp"
#include <cassert>
int main()
{
    assert(StringUtils::lower("PyThOn") == "python");
    return 0;
}
