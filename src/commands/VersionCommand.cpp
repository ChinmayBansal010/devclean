#include "commands/VersionCommand.hpp"

#include <iostream>

int VersionCommand::execute(const ParsedArgs&)
{
    std::cout << "devclean v0.1.0\n";
    return 0;
}