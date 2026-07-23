#pragma once

#include "commands/ICommand.hpp"

class VersionCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};