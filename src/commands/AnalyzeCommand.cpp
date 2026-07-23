#include "commands/AnalyzeCommand.hpp"

#include "core/Config.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
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

std::string escapeCsv(const std::string& value)
{
    std::string escaped = value;
    std::replace(escaped.begin(), escaped.end(), '"', '\"');
    return escaped;
}

std::vector<ScanResult> applyFilters(const std::vector<ScanResult>& input, const ParsedArgs& args)
{
    std::vector<ScanResult> results = input;
    if (!args.category.empty())
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return result.category != args.category;
        }), results.end());
    }
    return results;
}

void printMarkdownReport(const std::vector<ScanResult>& results)
{
    std::cout << "# devclean analysis\n\n";
    std::cout << "## Largest caches\n";
    for (const auto& result : results)
    {
        if (result.found)
            std::cout << "- " << result.name << " - " << Formatter::formatBytes(result.bytes) << "\n";
    }
    std::cout << "\n## Recommendations\n";
    for (const auto& result : results)
    {
        if (result.found && result.bytes > 1024 * 1024 * 50)
            std::cout << "- Consider pruning " << result.name << " because it is large.\n";
    }
}

void printHtmlReport(const std::vector<ScanResult>& results)
{
    std::cout << "<html><body><h1>devclean analysis</h1><ul>";
    for (const auto& result : results)
    {
        if (result.found)
            std::cout << "<li>" << result.name << " - " << Formatter::formatBytes(result.bytes) << "</li>";
    }
    std::cout << "</ul></body></html>\n";
}

void printCsvReport(const std::vector<ScanResult>& results)
{
    std::cout << "name,bytes,category\n";
    for (const auto& result : results)
    {
        std::cout << escapeCsv(result.name) << ',' << result.bytes << ',' << escapeCsv(result.category) << '\n';
    }
}

void printJsonReport(const std::vector<ScanResult>& results)
{
    nlohmann::json payload = nlohmann::json::array();
    for (const auto& result : results)
    {
        nlohmann::json entry = nlohmann::json::object();
        entry["name"] = result.name;
        entry["bytes"] = result.bytes;
        entry["category"] = result.category;
        entry["found"] = result.found;
        payload.push_back(std::move(entry));
    }
    std::cout << payload.dump(2) << '\n';
}

} // namespace

int AnalyzeCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ScannerEngine engine;
    auto results = engine.scan(args.targets, config);
    auto filtered = applyFilters(results, args);

    std::sort(filtered.begin(), filtered.end(), [](const ScanResult& lhs, const ScanResult& rhs) {
        return lhs.bytes > rhs.bytes;
    });

    if (!args.reportFormat.empty())
    {
        const std::string format = normalize(args.reportFormat);
        if (format == "markdown")
            printMarkdownReport(filtered);
        else if (format == "html")
            printHtmlReport(filtered);
        else if (format == "csv")
            printCsvReport(filtered);
        else if (format == "json")
            printJsonReport(filtered);
        else
            std::cout << "Unsupported report format: " << args.reportFormat << '\n';
        return 0;
    }

    std::cout << "\nAnalysis\n";
    std::cout << "--------\n";
    std::cout << "Largest caches:\n";
    for (std::size_t i = 0; i < std::min<std::size_t>(filtered.size(), 8); ++i)
    {
        const auto& result = filtered[i];
        std::cout << "- " << result.name << " : " << Formatter::formatBytes(result.bytes) << "\n";
    }
    std::cout << "\nRecommendations:\n";
    for (const auto& result : filtered)
    {
        if (result.found && result.bytes > 1024 * 1024 * 25)
            std::cout << "- Large cache: " << result.name << "\n";
    }

    return 0;
}
