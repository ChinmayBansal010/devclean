#include "core/ArgumentParser.hpp"
#include <cassert>
int main()
{
    char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("--version")};
    const auto args = ArgumentParser::parse(2, argv);
    assert(args.version);
    return 0;
}
