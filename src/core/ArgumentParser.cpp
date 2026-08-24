#include "core/ArgumentParser.hpp"

#include "utils/StringUtils.hpp"

#include <cctype>
#include <limits>
#include <string>

namespace {

bool isOption(const std::string& token)
{
    return StringUtils::startsWith(token, "--") || token == "-h" || token == "-V";
}

bool hasValue(int index, int argc, char* argv[])
{
    return index + 1 < argc && !isOption(argv[index + 1]);
}

bool inlineValue(const std::string& token, const std::string& option, std::string& value)
{
    const std::string prefix = option + "=";
    if (!StringUtils::startsWith(token, prefix))
        return false;
    value = token.substr(prefix.size());
    return true;
}

uint64_t parseSize(const std::string& value)
{
    const std::string normalized = StringUtils::lower(value);
    std::size_t suffixStart = 0;
    while (suffixStart < normalized.size() && std::isdigit(static_cast<unsigned char>(normalized[suffixStart]))) ++suffixStart;
    if (suffixStart == 0) return 0;
    uint64_t multiplier = 1;
    const std::string suffix = normalized.substr(suffixStart);
    if (suffix == "kb") multiplier = 1024ULL;
    else if (suffix == "mb") multiplier = 1024ULL * 1024ULL;
    else if (suffix == "gb") multiplier = 1024ULL * 1024ULL * 1024ULL;
    else if (suffix == "tb") multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    else if (!suffix.empty() && suffix != "b") return 0;
    try
    {
        const uint64_t base = std::stoull(normalized.substr(0, suffixStart));
        if (base > std::numeric_limits<uint64_t>::max() / multiplier) return 0;
        return base * multiplier;
    }
    catch (...) { return 0; }
}

uint64_t parseDuration(const std::string& value)
{
    const std::string normalized = StringUtils::lower(value);
    std::size_t suffixStart = 0;
    while (suffixStart < normalized.size() && std::isdigit(static_cast<unsigned char>(normalized[suffixStart]))) ++suffixStart;
    if (suffixStart == 0) return 0;
    uint64_t multiplier = 1;
    const std::string suffix = normalized.substr(suffixStart);
    if (suffix == "s") multiplier = 1;
    else if (suffix == "m") multiplier = 60ULL;
    else if (suffix == "h") multiplier = 60ULL * 60ULL;
    else if (suffix == "d") multiplier = 24ULL * 60ULL * 60ULL;
    else if (suffix == "w") multiplier = 7ULL * 24ULL * 60ULL * 60ULL;
    else return 0;
    try
    {
        const uint64_t base = std::stoull(normalized.substr(0, suffixStart));
        if (base > std::numeric_limits<uint64_t>::max() / multiplier) return 0;
        return base * multiplier;
    }
    catch (...) { return 0; }
}

}

ParsedArgs ArgumentParser::parse(int argc, char* argv[])
{
    ParsedArgs args;
    for (int i = 1; i < argc; ++i)
    {
        const std::string token = argv[i];
        const std::string lower = StringUtils::lower(token);
        std::string value;
        if (token == "--json") args.json = true;
        else if (token == "--verbose") args.verbose = true;
        else if (token == "--dry-run") args.dryRun = true;
        else if (token == "--force") args.force = true;
        else if (token == "--safe") args.safe = true;
        else if (token == "--active-only") args.activeOnly = true;
        else if (token == "--min-size" || inlineValue(token, "--min-size", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.minSizeBytes = parseSize(value.empty() ? argv[++i] : value);
        }
        else if (token == "--max-size" || inlineValue(token, "--max-size", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.maxSizeBytes = parseSize(value.empty() ? argv[++i] : value);
        }
        else if (token == "--target" || inlineValue(token, "--target", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.targetSizeBytes = parseSize(value.empty() ? argv[++i] : value);
        }
        else if (token == "--stale" || inlineValue(token, "--stale", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.staleSeconds = parseDuration(value.empty() ? argv[++i] : value);
        }
        else if (token == "--help" || token == "-h") args.help = true;
        else if (token == "--version" || token == "-V") args.version = true;
        else if (token == "--category" || inlineValue(token, "--category", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.category = StringUtils::lower(value.empty() ? argv[++i] : value);
        }
        else if (token == "--exclude" || inlineValue(token, "--exclude", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.excludes.emplace_back(StringUtils::lower(value.empty() ? argv[++i] : value));
        }
        else if (token == "--sort" || inlineValue(token, "--sort", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.sort = StringUtils::lower(value.empty() ? argv[++i] : value);
        }
        else if (token == "--report" || inlineValue(token, "--report", value))
        {
            if (!value.empty() || hasValue(i, argc, argv)) args.reportFormat = StringUtils::lower(value.empty() ? argv[++i] : value);
        }
        else if (token == "--reverse") args.reverse = true;
        else if (!isOption(token))
        {
            if (args.command.empty()) args.command = lower;
            else args.targets.emplace_back(lower);
        }
    }
    return args;
}
