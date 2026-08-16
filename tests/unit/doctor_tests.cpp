#include "platform/ToolDetector.hpp"

#include <cassert>

int main()
{
    const auto report = ToolDetector::getInstance().getEnvironmentReport();
    assert(!report.empty());
    for (const auto& tool : report)
    {
        assert(!tool.name.empty());
        if (tool.running)
            assert(tool.installed || tool.name == "code" || tool.name == "idea");
    }
    return 0;
}
