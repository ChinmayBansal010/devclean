#include "core/ArgumentParser.hpp"
#include <cassert>
int main()
{
    char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("clean"), const_cast<char*>("--force")};
    const auto args = ArgumentParser::parse(3, argv);
    assert(args.force);
    return 0;
}
