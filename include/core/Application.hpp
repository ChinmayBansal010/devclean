#pragma once

#include <csignal>

class Application
{
public:
    int run(int argc, char* argv[]);
    static bool isInterrupted();

private:
    static volatile sig_atomic_t interrupted;
    static void handleSignal(int signal);
};
