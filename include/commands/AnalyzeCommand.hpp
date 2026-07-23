#pragma once

#include "commands/ICommand.hpp"

class AnalyzeCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};
