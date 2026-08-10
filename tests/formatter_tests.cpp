#include "utils/Formatter.hpp"

#include <cassert>
#include <filesystem>
#include <string>

int main()
{
    assert(Formatter::formatBytes(0) == "0.00 B");
    assert(Formatter::formatBytes(1024) == "1.00 KB");
    assert(Formatter::formatBytes(1024ULL * 1024ULL) == "1.00 MB");
    assert(Formatter::formatBytes(1024ULL * 1024ULL * 1024ULL) == "1.00 GB");

    assert(Formatter::formatPath({}) == "<unset>");
    const std::filesystem::path path = std::filesystem::path("cache") / "pip";
    assert(Formatter::formatPath(path) == path.string());

    return 0;
}
