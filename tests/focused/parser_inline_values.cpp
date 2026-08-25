#include "core/ArgumentParser.hpp"

#include <cassert>

int main()
{
    char* argv[] = {
        const_cast<char*>("devclean"), const_cast<char*>("clean"),
        const_cast<char*>("--target=5GB"), const_cast<char*>("--stale=2h"),
        const_cast<char*>("--category=PYTHON")
    };

    const ParsedArgs args = ArgumentParser::parse(5, argv);
    assert(args.targetSizeBytes == 5ULL * 1024ULL * 1024ULL * 1024ULL);
    assert(args.staleSeconds == 2ULL * 60ULL * 60ULL);
    assert(args.category == "python");
    return 0;
}
