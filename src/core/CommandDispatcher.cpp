#include "core/CommandDispatcher.hpp"

#include "commands/AnalyzeCommand.hpp"
#include "commands/CleanCommand.hpp"
#include "commands/ScanCommand.hpp"
#include "commands/StatsCommand.hpp"
#include "commands/VersionCommand.hpp"
#include "core/ArgumentParser.hpp"

#include <iostream>
#include <string>

namespace {

void printHelp()
{
    std::cout << "devclean - scan and clean developer cache directories\n";
    std::cout << "Usage: devclean <command> [options] [targets]\n";
    std::cout << "\nCommands:\n";
    std::cout << "  scan         Scan known developer caches\n";
    std::cout << "  analyze      Analyze cache growth and recommendations\n";
    std::cout << "  clean        Remove discovered cache directories\n";
    std::cout << "  stats        Show aggregate cache statistics\n";
    std::cout << "  version      Print the current version\n";
    std::cout << "\nTargets:\n";
    std::cout << "  python, rust, javascript, java, cpp, containers, editor, build, package managers\n";
    std::cout << "  Any cache name or alias such as pip, poetry, uv, cargo, npm, pnpm, gradle, maven, ccache, conan, vcpkg\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --json              Emit JSON output\n";
    std::cout << "  --verbose           Include additional details\n";
    std::cout << "  --dry-run           Show what would be removed\n";
    std::cout << "  --force             Skip confirmation prompts\n";
    std::cout << "  --category <name>   Filter by cache category\n";
    std::cout << "  --exclude <name>    Exclude a cache by name/alias\n";
    std::cout << "  --sort <name|size|modified>  Sort results\n";
    std::cout << "  --reverse           Reverse the current sort order\n";
    std::cout << "  --report <markdown|html|csv|json>  Emit a report in the requested format\n";
    std::cout << "  --help              Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  devclean scan\n";
    std::cout << "  devclean analyze --report markdown\n";
    std::cout << "  devclean clean --dry-run --exclude npm\n";
    std::cout << "  devclean analyze --category build --report json\n";
    std::cout << "  devclean stats --json\n";
}

} // namespace

int CommandDispatcher::dispatch(int argc, char* argv[])
{
    const ParsedArgs args = ArgumentParser::parse(argc, argv);

    if (args.command.empty() || args.help)
    {
        printHelp();
        return args.help ? 0 : 0;
    }

    if (args.command == "scan")
    {
        ScanCommand command;
        return command.execute(args);
    }

    if (args.command == "analyze")
    {
        AnalyzeCommand command;
        return command.execute(args);
    }

    if (args.command == "clean")
    {
        CleanCommand command;
        return command.execute(args);
    }

    if (args.command == "stats")
    {
        StatsCommand command;
        return command.execute(args);
    }

    if (args.command == "version")
    {
        VersionCommand command;
        return command.execute(args);
    }

    if (args.command == "help")
    {
        printHelp();
        return 0;
    }

    std::cout << "Unknown command: " << args.command << '\n';
    return 1;
}
