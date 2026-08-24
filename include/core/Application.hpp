#pragma once

#include <csignal>

class Application
{
public:
    int run(int argc, char* argv[]);
    static bool isInterrupted();
    static void resetInterrupt();

private:
    static volatile sig_atomic_t interrupted;
    static void handleSignal(int signal);
};
