#pragma once

#include "commands/ICommand.hpp"

class DashboardCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};
