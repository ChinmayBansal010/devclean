#include "commands/DoctorCommand.hpp"

#include "platform/ToolDetector.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

int DoctorCommand::execute(const ParsedArgs& args)
{
    const auto report = ToolDetector::getInstance().getEnvironmentReport();

    if (args.json)
    {
        nlohmann::json payload = nlohmann::json::object();
        payload["command"] = "doctor";
        payload["tools"] = nlohmann::json::array();
        for (const auto& tool : report)
        {
            payload["tools"].push_back({
                {"name", tool.name},
                {"installed", tool.installed},
                {"running", tool.running}
            });
        }
        std::cout << payload.dump(2) << '\n';
        return 0;
    }

    std::cout << "Developer environment\n";
    std::cout << "----------------------\n";
    for (const auto& tool : report)
    {
        const char* state = tool.installed ? "installed" : "missing";
        std::cout << "  " << tool.name << "\t" << state;
        if (tool.running)
            std::cout << " (running)";
        std::cout << '\n';
    }

    return 0;
}
