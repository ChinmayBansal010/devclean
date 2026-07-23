#include "core/Application.hpp"
#include "core/CommandDispatcher.hpp"

int Application::run(int argc, char* argv[])
{
    CommandDispatcher dispatcher;
    return dispatcher.dispatch(argc, argv);
}