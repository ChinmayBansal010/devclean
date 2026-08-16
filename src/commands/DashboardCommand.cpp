#include "commands/DashboardCommand.hpp"

#include "core/CacheInsights.hpp"
#include "core/Config.hpp"
#include "core/ScanHistory.hpp"
#include "scanner/ScannerEngine.hpp"
#include "utils/Formatter.hpp"
#include "utils/TerminalUtils.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace {

enum class Key
{
    none,
    quit,
    refresh,
    up,
    down,
    home,
    end,
    pageUp,
    pageDown,
    toggleAuto,
    nextRecommendation,
    prevRecommendation,
};

std::string normalize(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string canonicalCategory(const std::string& value)
{
    const std::string normalized = normalize(value);
    if (normalized == "node")
        return "javascript";
    if (normalized == "package-managers")
        return "package managers";
    return normalized;
}

std::vector<ScanResult> applyFilters(std::vector<ScanResult> results, const ParsedArgs& args)
{
    if (args.activeOnly)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [](const ScanResult& result) {
            return !result.active;
        }), results.end());
    }

    if (args.minSizeBytes > 0)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return !result.found || result.bytes < args.minSizeBytes;
        }), results.end());
    }

    if (args.maxSizeBytes > 0)
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return !result.found || result.bytes > args.maxSizeBytes;
        }), results.end());
    }

    if (!args.category.empty())
    {
        const std::string category = canonicalCategory(args.category);
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            return canonicalCategory(result.category) != category;
        }), results.end());
    }

    if (!args.excludes.empty())
    {
        results.erase(std::remove_if(results.begin(), results.end(), [&](const ScanResult& result) {
            const std::string normalizedName = normalize(result.name);
            return std::any_of(args.excludes.begin(), args.excludes.end(), [&](const std::string& excluded) {
                return normalizedName == normalize(excluded);
            });
        }), results.end());
    }

    return results;
}

std::string repeat(char ch, std::size_t count)
{
    return std::string(count, ch);
}

std::string padRight(const std::string& value, std::size_t width)
{
    if (value.size() >= width)
        return value.substr(0, width);
    return value + repeat(' ', width - value.size());
}

std::vector<std::string> wrapText(const std::string& text, std::size_t width)
{
    std::vector<std::string> lines;
    if (text.empty())
    {
        lines.emplace_back();
        return lines;
    }

    std::istringstream stream(text);
    std::string word;
    std::string line;

    while (stream >> word)
    {
        if (line.empty())
        {
            line = word;
            continue;
        }

        if (line.size() + 1 + word.size() <= width)
        {
            line += ' ';
            line += word;
        }
        else
        {
            lines.push_back(line);
            line = word;
        }
    }

    if (!line.empty())
        lines.push_back(line);

    return lines;
}

std::string formatSignedBytes(int64_t bytes)
{
    const std::string sign = bytes >= 0 ? "+" : "-";
    const uint64_t magnitude = static_cast<uint64_t>(bytes >= 0 ? bytes : -bytes);
    return sign + Formatter::formatBytes(magnitude);
}

std::string formatSignedPercent(double value)
{
    std::ostringstream out;
    if (value >= 0.0)
        out << '+';
    out << std::fixed << std::setprecision(1) << value << '%';
    return out.str();
}

std::string makeSparkline(const std::vector<CacheTrendPoint>& history, std::size_t width)
{
    if (history.empty())
        return "no history yet";

    std::vector<uint64_t> values;
    values.reserve(history.size());
    for (auto it = history.rbegin(); it != history.rend(); ++it)
        values.push_back(it->totalBytes);

    const std::size_t sampleCount = std::min(width, values.size());
    const std::size_t offset = values.size() - sampleCount;

    uint64_t minValue = values[offset];
    uint64_t maxValue = values[offset];
    for (std::size_t i = offset; i < values.size(); ++i)
    {
        minValue = std::min(minValue, values[i]);
        maxValue = std::max(maxValue, values[i]);
    }

    static constexpr const char* levels = "▁▂▃▄▅▆▇█";
    std::string sparkline;
    sparkline.reserve(sampleCount);
    for (std::size_t i = offset; i < values.size(); ++i)
    {
        const uint64_t value = values[i];
        const std::size_t level = (maxValue == minValue)
            ? 7
            : static_cast<std::size_t>(std::round(
                  (static_cast<double>(value - minValue) * 7.0) /
                  static_cast<double>(maxValue - minValue)));
        sparkline += levels[std::min<std::size_t>(level, 7)];
    }

    return sparkline;
}

std::string repeatToken(const std::string& token, std::size_t count)
{
    std::string result;
    result.reserve(token.size() * count);
    for (std::size_t i = 0; i < count; ++i)
        result += token;
    return result;
}

std::vector<std::string> makeBox(const std::string& title,
                                 const std::vector<std::string>& lines,
                                 std::size_t width,
                                 bool unicode)
{
    const std::size_t innerWidth = width >= 2 ? width - 2 : width;
    const char* topLeft = unicode ? "┌" : "+";
    const char* topRight = unicode ? "┐" : "+";
    const char* bottomLeft = unicode ? "└" : "+";
    const char* bottomRight = unicode ? "┘" : "+";
    const char* horizontal = unicode ? "─" : "-";
    const char* vertical = unicode ? "│" : "|";

    std::vector<std::string> output;
    output.reserve(lines.size() + 3);
    output.push_back(std::string(topLeft) + repeatToken(horizontal, innerWidth) + topRight);

    const std::string titleLine = " " + title;
    output.push_back(std::string(vertical) + padRight(titleLine, innerWidth) + vertical);

    if (!lines.empty())
    {
        output.push_back(std::string(vertical) + repeat(' ', innerWidth) + vertical);
        for (const auto& line : lines)
            output.push_back(std::string(vertical) + padRight(line, innerWidth) + vertical);
    }
    else
    {
        output.push_back(std::string(vertical) + repeat(' ', innerWidth) + vertical);
    }

    output.push_back(std::string(bottomLeft) + repeatToken(horizontal, innerWidth) + bottomRight);
    return output;
}

std::vector<std::string> makeRecommendationLines(const CacheInsights& insights, std::size_t selected, std::size_t width)
{
    std::vector<std::string> lines;
    if (insights.recommendations.empty())
    {
        lines.push_back("No strong cleanup recommendations yet.");
        return lines;
    }

    const std::size_t count = std::min<std::size_t>(insights.recommendations.size(), 6);
    for (std::size_t i = 0; i < count; ++i)
    {
        const auto& recommendation = insights.recommendations[i];
        std::ostringstream header;
        header << (i == selected ? "▶ " : "  ");
        header << recommendation.name << "  " << Formatter::formatBytes(recommendation.bytes);
        if (recommendation.safe)
            header << "  safe";
        lines.push_back(header.str());

        auto reasonLines = wrapText("Reason: " + recommendation.reason, width);
        lines.insert(lines.end(), reasonLines.begin(), reasonLines.end());

        auto actionLines = wrapText("Action: " + recommendation.action, width);
        lines.insert(lines.end(), actionLines.begin(), actionLines.end());

        if (i + 1 < count)
            lines.emplace_back();
    }

    return lines;
}

std::vector<std::string> makeLargestCacheLines(const CacheInsights& insights, std::size_t width)
{
    std::vector<std::string> lines;
    if (insights.largestCaches.empty())
    {
        lines.push_back("No caches discovered.");
        return lines;
    }

    const std::size_t count = std::min<std::size_t>(insights.largestCaches.size(), 6);
    for (std::size_t i = 0; i < count; ++i)
    {
        std::ostringstream row;
        row << (i + 1) << ". " << insights.largestCaches[i].first << "  " << Formatter::formatBytes(insights.largestCaches[i].second);
        auto wrapped = wrapText(row.str(), width);
        lines.insert(lines.end(), wrapped.begin(), wrapped.end());
    }

    return lines;
}

std::vector<std::string> makeSummaryLines(const CacheInsights& insights, const ParsedArgs& args, std::size_t refreshCount)
{
    std::vector<std::string> lines;
    lines.push_back("Health score      " + std::to_string(insights.health.score) + "/100  " + insights.health.label);
    lines.push_back("Total cache usage " + Formatter::formatBytes(insights.totalBytes));
    lines.push_back("Found caches      " + std::to_string(insights.foundCount));
    lines.push_back("Active caches      " + std::to_string(insights.activeCount));
    lines.push_back("Warnings           " + std::to_string(insights.warningCount));
    lines.push_back("Latest growth      " + formatSignedBytes(insights.latestGrowthBytes) + "  (" + formatSignedPercent(insights.latestGrowthPercent) + ")");
    lines.push_back("Average growth     " + formatSignedBytes(insights.averageGrowthBytes) + " per snapshot");
    lines.push_back("Refresh count      " + std::to_string(refreshCount));
    lines.push_back("");
    lines.push_back("Filters");
    lines.push_back("Active only        " + std::string(args.activeOnly ? "yes" : "no"));
    lines.push_back("Category           " + std::string(args.category.empty() ? "all" : args.category));
    lines.push_back("Min size           " + std::string(args.minSizeBytes > 0 ? Formatter::formatBytes(args.minSizeBytes) : "none"));
    lines.push_back("Max size           " + std::string(args.maxSizeBytes > 0 ? Formatter::formatBytes(args.maxSizeBytes) : "none"));
    lines.push_back("");
    lines.push_back("Trend");
    lines.push_back(makeSparkline(insights.history, 24));

    if (!insights.health.factors.empty())
    {
        lines.push_back("");
        lines.push_back("Health factors");
        for (const auto& factor : insights.health.factors)
        {
            const auto wrapped = wrapText("- " + factor, 48);
            lines.insert(lines.end(), wrapped.begin(), wrapped.end());
        }
    }

    return lines;
}

std::vector<std::string> makeDetailLines(const CacheInsights& insights, std::size_t selected, std::size_t width)
{
    std::vector<std::string> lines;
    if (insights.recommendations.empty())
    {
        lines.push_back("No recommendation selected.");
        return lines;
    }

    const std::size_t index = std::min(selected, insights.recommendations.size() - 1);
    const auto& recommendation = insights.recommendations[index];

    lines.push_back("Selected recommendation");
    lines.push_back(recommendation.name + "  " + Formatter::formatBytes(recommendation.bytes));
    lines.push_back("Priority " + std::to_string(recommendation.priority) + (recommendation.safe ? "  safe" : ""));
    lines.push_back("");
    auto reasonLines = wrapText("Reason: " + recommendation.reason, width);
    lines.insert(lines.end(), reasonLines.begin(), reasonLines.end());
    lines.push_back("");
    auto actionLines = wrapText("Action: " + recommendation.action, width);
    lines.insert(lines.end(), actionLines.begin(), actionLines.end());
    return lines;
}

std::string color(const std::string& text, const char* code, bool enabled)
{
    if (!enabled)
        return text;
    return std::string(code) + text + "\033[0m";
}

std::string truncateToWidth(const std::string& text, std::size_t width)
{
    if (text.size() <= width)
        return text;
    if (width <= 3)
        return text.substr(0, width);
    return text.substr(0, width - 3) + "...";
}

std::vector<std::string> buildDashboardContent(const CacheInsights& insights,
                                                const ParsedArgs& args,
                                                std::size_t refreshCount,
                                                bool autoRefresh,
                                                bool unicode,
                                                std::size_t width,
                                                std::size_t selectedRecommendation)
{
    const std::size_t panelWidth = width > 4 ? width - 4 : width;

    std::vector<std::string> lines;
    lines.push_back(truncateToWidth("devclean dashboard", width));
    lines.push_back(truncateToWidth("Health " + std::to_string(insights.health.score) + "/100  " + insights.health.label +
                          " | Total " + Formatter::formatBytes(insights.totalBytes) +
                          " | Latest " + formatSignedBytes(insights.latestGrowthBytes) +
                          " | Refresh " + std::to_string(refreshCount) +
                          " | " + (autoRefresh ? "auto" : "paused"), width));
    lines.push_back("");

    auto overview = makeBox("Overview", makeSummaryLines(insights, args, refreshCount), panelWidth, unicode);
    lines.insert(lines.end(), overview.begin(), overview.end());
    lines.push_back("");

    auto recommendations = makeBox("Recommendations", makeRecommendationLines(insights, selectedRecommendation, panelWidth - 4), panelWidth, unicode);
    lines.insert(lines.end(), recommendations.begin(), recommendations.end());
    lines.push_back("");

    auto detail = makeBox("Focused detail", makeDetailLines(insights, selectedRecommendation, panelWidth - 4), panelWidth, unicode);
    lines.insert(lines.end(), detail.begin(), detail.end());
    lines.push_back("");

    auto largest = makeBox("Largest caches", makeLargestCacheLines(insights, panelWidth - 4), panelWidth, unicode);
    lines.insert(lines.end(), largest.begin(), largest.end());
    lines.push_back("");

    lines.push_back(truncateToWidth("Controls: q quit  r refresh  j/k or arrows scroll  a toggle auto-refresh  n/p select recommendation  home/end jump", width));
    lines.push_back(truncateToWidth("Use scroll if the dashboard is taller than your terminal.", width));
    return lines;
}

void renderViewport(const std::vector<std::string>& lines,
                    std::size_t viewportOffset,
                    std::size_t viewportHeight,
                    std::size_t totalHeight)
{
    const std::size_t maxOffset = lines.size() > viewportHeight ? lines.size() - viewportHeight : 0;
    viewportOffset = std::min(viewportOffset, maxOffset);

    for (std::size_t i = 0; i < viewportHeight; ++i)
    {
        const std::size_t lineIndex = viewportOffset + i;
        if (lineIndex < lines.size())
            std::cout << lines[lineIndex];
        std::cout << '\n';
    }

    if (totalHeight > viewportHeight)
    {
        const double position = maxOffset == 0 ? 1.0 : static_cast<double>(viewportOffset) / static_cast<double>(maxOffset);
        const std::size_t barWidth = 18;
        const std::size_t filled = static_cast<std::size_t>(std::round(position * static_cast<double>(barWidth)));
        std::string bar = "[";
        for (std::size_t i = 0; i < barWidth; ++i)
            bar += i < filled ? '=' : '-';
        bar += "]";
        std::cout << "Scroll " << bar << " " << std::to_string(viewportOffset + 1) << "-" << std::to_string(std::min(lines.size(), viewportOffset + viewportHeight)) << " / " << lines.size() << '\n';
    }
    else
    {
        std::cout << "Scroll [------------------] 1-" << lines.size() << " / " << lines.size() << '\n';
    }
}

class TerminalSession
{
public:
    TerminalSession()
    {
        if (!TerminalUtils::isTTY())
            return;

        enabled = true;

#ifdef _WIN32
        outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        inputHandle = GetStdHandle(STD_INPUT_HANDLE);
        if (outputHandle != INVALID_HANDLE_VALUE)
        {
            DWORD mode = 0;
            if (GetConsoleMode(outputHandle, &mode))
            {
                outputMode = mode;
                SetConsoleMode(outputHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            }
        }
        if (inputHandle != INVALID_HANDLE_VALUE)
        {
            DWORD mode = 0;
            if (GetConsoleMode(inputHandle, &mode))
            {
                inputMode = mode;
                mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
                SetConsoleMode(inputHandle, mode);
            }
        }
#else
        if (tcgetattr(STDIN_FILENO, &originalTermios) == 0)
        {
            termios raw = originalTermios;
            raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0)
                rawEnabled = true;
        }
#endif

        std::cout << "\033[?1049h\033[?25l";
    }

    ~TerminalSession()
    {
        if (!enabled)
            return;

        std::cout << "\033[0m\033[?25h\033[?1049l" << std::flush;

#ifdef _WIN32
        if (outputHandle != INVALID_HANDLE_VALUE && outputMode != 0)
            SetConsoleMode(outputHandle, outputMode);
        if (inputHandle != INVALID_HANDLE_VALUE && inputMode != 0)
            SetConsoleMode(inputHandle, inputMode);
#else
        if (rawEnabled)
            tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
#endif
    }

private:
    bool enabled = false;
#ifdef _WIN32
    HANDLE inputHandle = INVALID_HANDLE_VALUE;
    HANDLE outputHandle = INVALID_HANDLE_VALUE;
    DWORD inputMode = 0;
    DWORD outputMode = 0;
#else
    termios originalTermios{};
    bool rawEnabled = false;
#endif
};

std::optional<Key> readKey(int timeoutMs)
{
#ifdef _WIN32
    const int stepMs = 20;
    int waited = 0;
    while (waited < timeoutMs)
    {
        if (_kbhit())
        {
            const int ch = _getch();
            if (ch == 'q' || ch == 'Q')
                return Key::quit;
            if (ch == 'r' || ch == 'R')
                return Key::refresh;
            if (ch == 'a' || ch == 'A' || ch == ' ')
                return Key::toggleAuto;
            if (ch == 'n' || ch == 'N')
                return Key::nextRecommendation;
            if (ch == 'p' || ch == 'P')
                return Key::prevRecommendation;
            if (ch == 'k' || ch == 'K')
                return Key::up;
            if (ch == 'j' || ch == 'J')
                return Key::down;
            if (ch == 224 || ch == 0)
            {
                const int ext = _getch();
                if (ext == 72)
                    return Key::up;
                if (ext == 80)
                    return Key::down;
                if (ext == 71)
                    return Key::home;
                if (ext == 79)
                    return Key::end;
                if (ext == 73)
                    return Key::pageUp;
                if (ext == 81)
                    return Key::pageDown;
            }
            return Key::none;
        }

        Sleep(stepMs);
        waited += stepMs;
    }
    return std::nullopt;
#else
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    const int ready = select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv);
    if (ready <= 0)
        return std::nullopt;

    char ch = 0;
    if (read(STDIN_FILENO, &ch, 1) != 1)
        return std::nullopt;

    if (ch == 'q' || ch == 'Q')
        return Key::quit;
    if (ch == 'r' || ch == 'R')
        return Key::refresh;
    if (ch == 'a' || ch == 'A' || ch == ' ')
        return Key::toggleAuto;
    if (ch == 'n' || ch == 'N')
        return Key::nextRecommendation;
    if (ch == 'p' || ch == 'P')
        return Key::prevRecommendation;
    if (ch == 'k' || ch == 'K')
        return Key::up;
    if (ch == 'j' || ch == 'J')
        return Key::down;
    if (ch != '\033')
        return Key::none;

    char seq[3] = {0, 0, 0};
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
        return Key::none;
    if (seq[0] != '[')
        return Key::none;
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
        return Key::none;

    if (seq[1] == 'A')
        return Key::up;
    if (seq[1] == 'B')
        return Key::down;
    if (seq[1] == 'H')
        return Key::home;
    if (seq[1] == 'F')
        return Key::end;

    if (seq[1] == '5' || seq[1] == '6')
    {
        if (read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~')
            return seq[1] == '5' ? Key::pageUp : Key::pageDown;
    }

    return Key::none;
#endif
}

void printStaticReport(const CacheInsights& insights, const ParsedArgs& args)
{
    const bool unicode = TerminalUtils::supportsUnicode();
    const std::size_t width = static_cast<std::size_t>(TerminalUtils::getTerminalWidth());

    const bool useColor = TerminalUtils::isTTY();
    std::cout << color("devclean dashboard", "\033[38;2;99;176;255m\033[1m", useColor) << "\n";
    std::cout << color("Health " + std::to_string(insights.health.score) + "/100  " + insights.health.label, "\033[38;2;170;255;195m", useColor) << "\n\n";

    auto content = buildDashboardContent(insights, args, 0, false, unicode, width, 0);
    for (const auto& line : content)
        std::cout << line << '\n';
}

void renderDashboardScreen(const CacheInsights& insights,
                           const ParsedArgs& args,
                           std::size_t refreshCount,
                           std::size_t selectedRecommendation,
                           bool autoRefresh,
                           bool unicode,
                           std::size_t width,
                           std::size_t viewportOffset,
                           std::size_t terminalHeight)
{
    std::cout << "\033[H\033[2J";
    auto content = buildDashboardContent(insights, args, refreshCount, autoRefresh, unicode, width, selectedRecommendation);
    const std::size_t viewportHeight = terminalHeight > 1 ? terminalHeight - 1 : terminalHeight;
    renderViewport(content, viewportOffset, viewportHeight, terminalHeight);
}

void printJsonReport(const CacheInsights& insights)
{
    nlohmann::json payload = nlohmann::json::object();
    payload["command"] = "dashboard";
    payload["health"] = {
        {"score", insights.health.score},
        {"label", insights.health.label},
        {"factors", insights.health.factors}
    };
    payload["totals"] = {
        {"bytes", insights.totalBytes},
        {"files", insights.totalFiles},
        {"directories", insights.totalDirectories},
        {"found_count", insights.foundCount},
        {"active_count", insights.activeCount},
        {"warning_count", insights.warningCount}
    };
    payload["trend"] = {
        {"latest_growth_bytes", insights.latestGrowthBytes},
        {"latest_growth_percent", insights.latestGrowthPercent},
        {"average_growth_bytes", insights.averageGrowthBytes}
    };
    payload["history"] = nlohmann::json::array();
    for (const auto& point : insights.history)
    {
        payload["history"].push_back({
            {"timestamp", point.label},
            {"total_bytes", point.totalBytes},
            {"delta_bytes", point.deltaBytes}
        });
    }
    payload["recommendations"] = nlohmann::json::array();
    for (const auto& recommendation : insights.recommendations)
    {
        payload["recommendations"].push_back({
            {"name", recommendation.name},
            {"reason", recommendation.reason},
            {"action", recommendation.action},
            {"bytes", recommendation.bytes},
            {"growth_bytes", recommendation.growthBytes},
            {"priority", recommendation.priority},
            {"safe", recommendation.safe}
        });
    }
    std::cout << payload.dump(2) << '\n';
}

struct DashboardState
{
    std::vector<ScanResult> results;
    CacheInsights insights;
};

DashboardState scanDashboard(ScannerEngine& scanner,
                             const ParsedArgs& args,
                             const AppConfig& config)
{
    DashboardState state;
    state.results = scanner.scan(args.targets, config);
    state.results = applyFilters(std::move(state.results), args);
    ScanHistory::getInstance().recordScan(state.results);
    state.insights = buildCacheInsights(state.results, ScanHistory::getInstance().getHistory(6));
    return state;
}

} // namespace

int DashboardCommand::execute(const ParsedArgs& args)
{
    AppConfig config = ConfigLoader::load();
    ParsedArgs effectiveArgs = args;
    if (effectiveArgs.category.empty() && !config.defaultCategory.empty())
        effectiveArgs.category = config.defaultCategory;

    ScannerEngine scanner;

    if (args.json)
    {
        auto state = scanDashboard(scanner, effectiveArgs, config);
        printJsonReport(state.insights);
        return 0;
    }

    if (!TerminalUtils::isTTY())
    {
        auto state = scanDashboard(scanner, effectiveArgs, config);
        printStaticReport(state.insights, effectiveArgs);
        return 0;
    }

    TerminalSession session;
    const bool unicode = TerminalUtils::supportsUnicode();
    const std::size_t width = static_cast<std::size_t>(TerminalUtils::getTerminalWidth());
    const std::size_t terminalHeight = static_cast<std::size_t>(TerminalUtils::getTerminalHeight());
    std::size_t refreshCount = 0;
    std::size_t selectedRecommendation = 0;
    std::size_t viewportOffset = 0;
    bool autoRefresh = false;
    auto state = scanDashboard(scanner, effectiveArgs, config);

    while (true)
    {
        if (state.insights.recommendations.empty())
            selectedRecommendation = 0;
        else if (selectedRecommendation >= state.insights.recommendations.size())
            selectedRecommendation = state.insights.recommendations.size() - 1;

        const auto content = buildDashboardContent(state.insights, effectiveArgs, refreshCount, autoRefresh, unicode, width, selectedRecommendation);
        const std::size_t viewportHeight = terminalHeight > 1 ? terminalHeight - 1 : terminalHeight;
        const std::size_t maxOffset = content.size() > viewportHeight ? content.size() - viewportHeight : 0;
        if (viewportOffset > maxOffset)
            viewportOffset = maxOffset;

        renderDashboardScreen(state.insights, effectiveArgs, refreshCount, selectedRecommendation, autoRefresh, unicode, width, viewportOffset, terminalHeight);

        const auto key = readKey(autoRefresh ? 15000 : 600000);
        if (!key)
        {
            if (autoRefresh)
            {
                state = scanDashboard(scanner, effectiveArgs, config);
                ++refreshCount;
            }
            continue;
        }

        switch (*key)
        {
        case Key::quit:
            return 0;
        case Key::refresh:
            state = scanDashboard(scanner, effectiveArgs, config);
            ++refreshCount;
            continue;
        case Key::toggleAuto:
            autoRefresh = !autoRefresh;
            continue;
        case Key::up:
            if (viewportOffset > 0)
                --viewportOffset;
            continue;
        case Key::down:
            if (viewportOffset < maxOffset)
                ++viewportOffset;
            continue;
        case Key::home:
            viewportOffset = 0;
            continue;
        case Key::end:
            viewportOffset = maxOffset;
            continue;
        case Key::pageUp:
            viewportOffset = viewportOffset > viewportHeight ? viewportOffset - viewportHeight : 0;
            continue;
        case Key::pageDown:
            viewportOffset = std::min(maxOffset, viewportOffset + viewportHeight);
            continue;
        case Key::nextRecommendation:
            if (!state.insights.recommendations.empty() && selectedRecommendation + 1 < state.insights.recommendations.size())
                ++selectedRecommendation;
            continue;
        case Key::prevRecommendation:
            if (selectedRecommendation > 0)
                --selectedRecommendation;
            continue;
        case Key::none:
            continue;
        }
    }
}
