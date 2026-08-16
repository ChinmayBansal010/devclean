#include "core/ArgumentParser.hpp"

#include <cassert>

int main()
{
    const auto longForm = ArgumentParser::parse({"devclean", "--version"});
    assert(longForm.version);

    const auto shortForm = ArgumentParser::parse({"devclean", "-V"});
    assert(shortForm.version);

    const auto normal = ArgumentParser::parse({"devclean", "scan"});
    assert(!normal.version);

    return 0;
}
