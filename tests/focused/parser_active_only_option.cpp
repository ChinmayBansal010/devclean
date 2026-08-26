#include "core/ArgumentParser.hpp"
#include <cassert>
int main()
{
    char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("scan"), const_cast<char*>("--active-only")};
    const auto args = ArgumentParser::parse(3, argv);
    assert(args.activeOnly);
    return 0;
}
