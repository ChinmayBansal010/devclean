#include "core/Application.hpp"
#include "core/CommandDispatcher.hpp"

#include <atomic>
#include <iostream>

volatile sig_atomic_t Application::interrupted = 0;

bool Application::isInterrupted()
{
    return interrupted != 0;
}

void Application::handleSignal(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
        interrupted = 1;
}

int Application::run(int argc, char* argv[])
{
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    CommandDispatcher dispatcher;
    int result = dispatcher.dispatch(argc, argv);

    if (interrupted)
    {
        std::cout << "\nOperation interrupted by user.\n";
        return 130;
    }

    return result;
}