#include "core/CommandDispatcher.hpp"

#include "commands/AnalyzeCommand.hpp"
#include "commands/DashboardCommand.hpp"
#include "commands/CleanCommand.hpp"
#include "commands/DoctorCommand.hpp"
#include "commands/RecommendCommand.hpp"
#include "commands/ScanCommand.hpp"
#include "commands/StatsCommand.hpp"
#include "commands/VersionCommand.hpp"
#include "core/ArgumentParser.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

void printHelp()
{
    std::cout << "devclean - scan and clean developer cache directories\n";
    std::cout << "Usage: devclean <command> [options] [targets]\n\nCommands:\n";
    std::cout << "  scan         Scan known developer caches\n  analyze      Analyze cache growth and recommendations\n";
    std::cout << "  recommend    Show intelligent cleanup recommendations\n  dashboard    Open the interactive cache health dashboard\n";
    std::cout << "  clean        Remove discovered cache directories\n  doctor       Inspect the developer environment\n";
    std::cout << "  stats        Show aggregate cache statistics\n  version      Print the current version\n";
    std::cout << "\nTargets:\n  python, rust, javascript, java, cpp, containers, editor, build, package managers\n";
    std::cout << "  Any cache name or alias such as pip, poetry, uv, cargo, npm, pnpm, gradle, maven, ccache, conan, vcpkg\n";
    std::cout << "\nOptions:\n  --json              Emit JSON output\n  --verbose           Include additional details\n";
    std::cout << "  --dry-run           Show what would be removed\n  --force             Skip confirmation prompts\n";
    std::cout << "  --safe              Only clean inactive, unprotected caches without warnings\n  --target <size>     Clean up to the requested amount, e.g. 10GB\n";
    std::cout << "  --stale <duration>  Remove files older than e.g. 30d, 12h, or 60m\n  --active-only       Include only caches associated with active tools\n";
    std::cout << "  --min-size <size>   Include caches at or above a size threshold\n  --max-size <size>   Include caches at or below a size threshold\n";
    std::cout << "  --category <name>   Filter by cache category\n  --exclude <name>    Exclude a cache by name/alias\n";
    std::cout << "  --sort <name|size|modified>  Sort results\n  --reverse           Reverse the current sort order\n";
    std::cout << "  --report <markdown|html|csv|json>  Emit a report in the requested format\n  --help, -h          Show this help message\n  --version, -V       Print the current version\n";
}

std::string canonicalCommand(const std::string& command)
{
    static const std::unordered_map<std::string, std::string> aliases = {
        {"s", "scan"}, {"a", "analyze"}, {"r", "recommend"}, {"c", "clean"},
        {"d", "doctor"}, {"st", "stats"}, {"v", "version"}
    };
    const auto it = aliases.find(command);
    return it == aliases.end() ? command : it->second;
}

std::string closestCommand(const std::string& command)
{
    static const std::vector<std::string> commands = {
        "scan", "analyze", "recommend", "dashboard", "clean", "doctor", "stats", "version", "help"
    };

    std::string best;
    std::size_t bestDistance = 3;
    for (const auto& candidate : commands)
    {
        const std::size_t common = std::min(command.size(), candidate.size());
        std::size_t distance = command.size() > candidate.size()
            ? command.size() - candidate.size()
            : candidate.size() - command.size();
        for (std::size_t i = 0; i < common; ++i)
            if (command[i] != candidate[i]) ++distance;
        if (distance < bestDistance)
        {
            bestDistance = distance;
            best = candidate;
        }
    }
    return best;
}

}

int CommandDispatcher::dispatch(int argc, char* argv[])
{
    ParsedArgs args = ArgumentParser::parse(argc, argv);
    args.command = canonicalCommand(args.command);

    if (args.version)
    {
        VersionCommand command;
        return command.execute(args);
    }
    if (args.command.empty() || args.help)
    {
        printHelp();
        return 0;
    }
    if (args.command == "scan") { ScanCommand command; return command.execute(args); }
    if (args.command == "analyze") { AnalyzeCommand command; return command.execute(args); }
    if (args.command == "recommend") { RecommendCommand command; return command.execute(args); }
    if (args.command == "dashboard") { DashboardCommand command; return command.execute(args); }
    if (args.command == "clean") { CleanCommand command; return command.execute(args); }
    if (args.command == "doctor") { DoctorCommand command; return command.execute(args); }
    if (args.command == "stats") { StatsCommand command; return command.execute(args); }
    if (args.command == "version") { VersionCommand command; return command.execute(args); }
    if (args.command == "help") { printHelp(); return 0; }

    std::cout << "Unknown command: " << args.command << '\n';
    const std::string suggestion = closestCommand(args.command);
    if (!suggestion.empty())
        std::cout << "Did you mean: " << suggestion << "?\n";
    return 1;
}
