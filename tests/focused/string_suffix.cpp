#include "utils/StringUtils.hpp"

#include <cassert>

int main()
{
    assert(StringUtils::endsWith("devclean.cpp", ".cpp"));
    assert(StringUtils::endsWith("cache", ""));
    assert(!StringUtils::endsWith("cache", ".json"));
    return 0;
}
