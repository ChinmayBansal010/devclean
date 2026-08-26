#include "core/ArgumentParser.hpp"
#include <cassert>
int main()
{
    char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("-h")};
    const auto args = ArgumentParser::parse(2, argv);
    assert(args.help);
    return 0;
}
