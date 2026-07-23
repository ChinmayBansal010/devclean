#pragma once

#include "core/ArgumentParser.hpp"

class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual int execute(const ParsedArgs& args) = 0;
};