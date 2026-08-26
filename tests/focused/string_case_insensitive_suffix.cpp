#include "utils/StringUtils.hpp"
#include <cassert>
int main()
{
    assert(StringUtils::equalsIgnoreCase("Python", "python"));
    return 0;
}
