#include "core/ArgumentParser.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
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
    return token.rfind("--", 0) == 0 || token == "-h" || token == "-V";
}

bool hasValue(int index, int argc, char* argv[])
{
    return index + 1 < argc && !isOption(argv[index + 1]);
}

uint64_t parseSize(const std::string& value)
{
    const std::string normalized = normalize(value);
    std::size_t suffixStart = 0;
    while (suffixStart < normalized.size() && std::isdigit(static_cast<unsigned char>(normalized[suffixStart])))
        ++suffixStart;

    if (suffixStart == 0)
        return 0;

    uint64_t multiplier = 1;
    const std::string suffix = normalized.substr(suffixStart);
    if (suffix == "kb")
        multiplier = 1024ULL;
    else if (suffix == "mb")
        multiplier = 1024ULL * 1024ULL;
    else if (suffix == "gb")
        multiplier = 1024ULL * 1024ULL * 1024ULL;
    else if (suffix == "tb")
        multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    else if (!suffix.empty() && suffix != "b")
        return 0;

    try
    {
        const uint64_t base = std::stoull(normalized.substr(0, suffixStart));
        if (base > std::numeric_limits<uint64_t>::max() / multiplier)
            return 0;
        return base * multiplier;
    }
    catch (...)
    {
        return 0;
    }
}

}

ParsedArgs ArgumentParser::parse(int argc, char* argv[])
{
    ParsedArgs args;

    for (int i = 1; i < argc; ++i)
    {
        std::string token = argv[i];
        std::string lower = normalize(token);

        if (token == "--json")
            args.json = true;
        else if (token == "--verbose")
            args.verbose = true;
        else if (token == "--dry-run")
            args.dryRun = true;
        else if (token == "--force")
            args.force = true;
        else if (token == "--active-only")
            args.activeOnly = true;
        else if (token == "--min-size")
        {
            if (hasValue(i, argc, argv))
                args.minSizeBytes = parseSize(argv[++i]);
        }
        else if (token == "--max-size")
        {
            if (hasValue(i, argc, argv))
                args.maxSizeBytes = parseSize(argv[++i]);
        }
        else if (token == "--help" || token == "-h")
            args.help = true;
        else if (token == "--version" || token == "-V")
            args.version = true;
        else if (token == "--category")
        {
            if (hasValue(i, argc, argv))
                args.category = normalize(argv[++i]);
        }
        else if (token == "--exclude")
        {
            if (hasValue(i, argc, argv))
                args.excludes.emplace_back(normalize(argv[++i]));
        }
        else if (token == "--sort")
        {
            if (hasValue(i, argc, argv))
                args.sort = normalize(argv[++i]);
        }
        else if (token == "--report")
        {
            if (hasValue(i, argc, argv))
                args.reportFormat = normalize(argv[++i]);
        }
        else if (token == "--reverse")
            args.reverse = true;
        else if (!isOption(token))
        {
            if (args.command.empty())
                args.command = lower;
            else
                args.targets.emplace_back(lower);
        }
    }

    return args;
}
