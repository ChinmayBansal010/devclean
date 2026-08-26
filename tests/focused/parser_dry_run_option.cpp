#include "core/ArgumentParser.hpp"
#include <cassert>
int main()
{
    char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("clean"), const_cast<char*>("--dry-run")};
    const auto args = ArgumentParser::parse(3, argv);
    assert(args.dryRun);
    return 0;
}
