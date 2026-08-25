#include "core/ArgumentParser.hpp"

#include <cassert>

int main()
{
    char* argv[] = {
        const_cast<char*>("devclean"), const_cast<char*>("clean"),
        const_cast<char*>("--stale"), const_cast<char*>(" 30d ")
    };

    const ParsedArgs args = ArgumentParser::parse(4, argv);
    assert(args.staleSeconds == 30ULL * 24ULL * 60ULL * 60ULL);
    return 0;
}
