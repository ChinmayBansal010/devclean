#include "core/ArgumentParser.hpp"

#include <cassert>

int main()
{
    char* argv[] = {
        const_cast<char*>("devclean"), const_cast<char*>("scan"),
        const_cast<char*>("--category"), const_cast<char*>("  Python  "),
        const_cast<char*>("project-a ")
    };

    const ParsedArgs args = ArgumentParser::parse(5, argv);
    assert(args.category == "python");
    assert(args.targets.size() == 1);
    assert(args.targets.front() == "project-a");
    return 0;
}
