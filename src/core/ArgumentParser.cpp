#include "core/ArgumentParser.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace {

std::string normalize(const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

bool isOption(const std::string& token)
{
    return token.rfind("--", 0) == 0;
}

} // namespace

ParsedArgs ArgumentParser::parse(int argc, char* argv[])
{
    ParsedArgs args;

    for (int i = 1; i < argc; ++i)
    {
        std::string token = argv[i];
        std::string lower = normalize(token);

        if (token == "--json")
        {
            args.json = true;
        }
        else if (token == "--verbose")
        {
            args.verbose = true;
        }
        else if (token == "--dry-run")
        {
            args.dryRun = true;
        }
        else if (token == "--force")
        {
            args.force = true;
        }
        else if (token == "--help" || token == "-h")
        {
            args.help = true;
        }
        else if (token == "--category")
        {
            if (i + 1 < argc)
                args.category = normalize(argv[++i]);
        }
        else if (token == "--exclude")
        {
            if (i + 1 < argc)
                args.excludes.emplace_back(normalize(argv[++i]));
        }
        else if (token == "--sort")
        {
            if (i + 1 < argc)
                args.sort = normalize(argv[++i]);
        }
        else if (token == "--report")
        {
            if (i + 1 < argc)
                args.reportFormat = normalize(argv[++i]);
        }
        else if (token == "--reverse")
        {
            args.reverse = true;
        }
        else if (!isOption(token))
        {
            if (args.command.empty())
            {
                args.command = lower;
            }
            else
            {
                args.targets.emplace_back(lower);
            }
        }
    }

    return args;
}
