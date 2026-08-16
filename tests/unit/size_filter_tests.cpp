#include "core/ArgumentParser.hpp"

#include <cassert>
#include <string>

int main()
{
    {
        const auto args = ArgumentParser::parse({"devclean", "scan", "--min-size", "250MB", "--max-size", "2GB"});
        assert(args.minSizeBytes.has_value());
        assert(args.maxSizeBytes.has_value());
        assert(*args.minSizeBytes == 250ULL * 1024ULL * 1024ULL);
        assert(*args.maxSizeBytes == 2ULL * 1024ULL * 1024ULL * 1024ULL);
        assert(*args.minSizeBytes < *args.maxSizeBytes);
    }

    {
        const auto args = ArgumentParser::parse({"devclean", "scan", "--min-size", "0B"});
        assert(args.minSizeBytes.has_value());
        assert(*args.minSizeBytes == 0);
    }

    return 0;
}
