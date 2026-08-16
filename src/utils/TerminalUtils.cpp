#include "utils/TerminalUtils.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

int TerminalUtils::getTerminalWidth()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        return std::max(20, static_cast<int>(info.srWindow.Right - info.srWindow.Left + 1));
    return 80;
#else
    if (isatty(STDOUT_FILENO))
    {
#ifdef TIOCGWINSZ
        winsize ws{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
            return std::max(20, static_cast<int>(ws.ws_col));
#endif
        const char* cols = std::getenv("COLUMNS");
        if (cols != nullptr)
        {
            try
            {
                return std::max(20, std::stoi(cols));
            }
            catch (const std::exception&)
            {
                return 80;
            }
        }
    }
    return 80;
#endif
}

int TerminalUtils::getTerminalHeight()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        return std::max(12, static_cast<int>(info.srWindow.Bottom - info.srWindow.Top + 1));
    return 24;
#else
    if (isatty(STDOUT_FILENO))
    {
#ifdef TIOCGWINSZ
        winsize ws{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0)
            return std::max(12, static_cast<int>(ws.ws_row));
#endif
        const char* lines = std::getenv("LINES");
        if (lines != nullptr)
        {
            try
            {
                return std::max(12, std::stoi(lines));
            }
            catch (const std::exception&)
            {
                return 24;
            }
        }
    }
    return 24;
#endif
}

bool TerminalUtils::supportsUnicode()
{
#ifdef _WIN32
    return false;
#else
    const char* lang = std::getenv("LANG");
    if (lang)
    {
        std::string langStr(lang);
        return langStr.find("UTF-8") != std::string::npos ||
               langStr.find("utf8") != std::string::npos;
    }
    return false;
#endif
}

bool TerminalUtils::isTTY()
{
#ifdef _WIN32
    return _isatty(_fileno(stdout)) != 0;
#else
    return isatty(STDOUT_FILENO) != 0;
#endif
}

std::string TerminalUtils::icon(const std::string& name)
{
    if (!supportsUnicode())
    {
        if (name == "check")
            return "[OK]";
        if (name == "cross")
            return "[X]";
        if (name == "arrow")
            return "->";
        if (name == "box")
            return "[#]";
        return "";
    }

    if (name == "check")
        return "✓";
    if (name == "cross")
        return "✗";
    if (name == "arrow")
        return "→";
    if (name == "box")
        return "■";
    return "";
}

ProgressBar::ProgressBar(uint64_t total, std::string_view label)
    : total(total), label(label), startTime(std::chrono::steady_clock::now())
{
}

ProgressBar::~ProgressBar() { }

void ProgressBar::update(uint64_t currentValue)
{
    current = std::min(currentValue, total);
    if (TerminalUtils::isTTY())
        render();
}

void ProgressBar::complete()
{
    current = total;
    if (TerminalUtils::isTTY())
        render();
    completed = true;
}

void ProgressBar::render()
{
    if (!TerminalUtils::isTTY())
        return;

    const double percentage = total == 0
        ? 1.0
        : static_cast<double>(current) / static_cast<double>(total);

    std::cout << "\r" << label << ": " << getProgressBar(percentage) << " "
              << (percentage * 100) << "%" << std::flush;

    if (current >= total)
        std::cout << "\n" << std::flush;
}

std::string ProgressBar::getProgressBar(double percentage)
{
    percentage = std::clamp(percentage, 0.0, 1.0);
    const int width = 30;
    const int filled = static_cast<int>(width * percentage);

    std::string bar = "[";
    for (int i = 0; i < width; ++i)
        bar += (i < filled) ? "=" : "-";
    bar += "]";

    return bar;
}

std::string ProgressBar::formatTime(std::chrono::duration<double> duration)
{
    auto seconds = static_cast<int>(duration.count());
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    std::string result;
    if (hours > 0)
        result += std::to_string(hours) + "h ";
    if (minutes > 0)
        result += std::to_string(minutes) + "m ";
    result += std::to_string(secs) + "s";

    return result;
}
