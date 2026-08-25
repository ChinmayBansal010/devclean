#include "utils/StringUtils.hpp"

#include <cassert>

int main()
{
    assert(StringUtils::endsWithIgnoreCase("cache.JSON", ".json"));
    assert(StringUtils::endsWithIgnoreCase("BUILD.CPP", ".cpp"));
    assert(!StringUtils::endsWithIgnoreCase("cache.bin", ".json"));
    return 0;
}
