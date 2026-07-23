#pragma once

#include "commands/ICommand.hpp"

class CleanCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};