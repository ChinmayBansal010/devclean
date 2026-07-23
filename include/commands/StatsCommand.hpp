#pragma once

#include "commands/ICommand.hpp"

class StatsCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};