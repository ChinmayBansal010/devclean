#include "commands/VersionCommand.hpp"

#include <iostream>

int VersionCommand::execute(const ParsedArgs&)
{
    std::cout << "devclean v" << DEVCLEAN_VERSION << '\n';
    return 0;
}
