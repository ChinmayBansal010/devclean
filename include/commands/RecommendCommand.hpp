#pragma once

#include "commands/ICommand.hpp"

class RecommendCommand : public ICommand
{
public:
    int execute(const ParsedArgs& args) override;
};
