#include "platform/ToolDetector.hpp"

#include <cassert>

int main()
{
    const auto report = ToolDetector::getInstance().getEnvironmentReport();
    assert(!report.empty());
    for (const auto& tool : report)
        assert(!tool.name.empty());
    return 0;
}
