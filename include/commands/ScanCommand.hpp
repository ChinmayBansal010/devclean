#pragma once

#include "commands/ICommand.hpp"

class ScanCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};