#pragma once

#include <chrono>
#include <cstdint>
#include <string>

class ProgressBar
{
public:
    ProgressBar(uint64_t total, std::string_view label = "Progress");
    ~ProgressBar();

    void update(uint64_t current);
    void complete();

    static bool supportsUnicode();
    static std::string getTerminalWidth();

private:
    uint64_t total;
    uint64_t current = 0;
    std::string label;
    std::chrono::steady_clock::time_point startTime;
    bool completed = false;
    bool jsonMode = false;

    void render();
    std::string getProgressBar(double percentage);
    std::string formatTime(std::chrono::duration<double> duration);
};

class TerminalUtils
{
public:
    static int getTerminalWidth();
    static bool supportsUnicode();
    static bool isTTY();

    static std::string icon(const std::string& name);
    static constexpr std::string_view CHECK = "✓";
    static constexpr std::string_view CROSS = "✗";
    static constexpr std::string_view ARROW = "→";
    static constexpr std::string_view BOX = "■";
};
