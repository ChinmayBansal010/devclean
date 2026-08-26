#include "core/ArgumentParser.hpp"
#include <cassert>
int main()
{
    char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("scan"), const_cast<char*>("--json")};
    const auto args = ArgumentParser::parse(3, argv);
    assert(args.json);
    assert(args.command == "scan");
    return 0;
}
