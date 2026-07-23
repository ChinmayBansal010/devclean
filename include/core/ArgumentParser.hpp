#pragma once

#include <string>
#include <vector>

struct ParsedArgs
{
    std::string command;
    std::vector<std::string> targets;
    std::vector<std::string> excludes;
    std::string category;
    std::string sort = "name";
    std::string reportFormat;
    bool reverse = false;
    bool json = false;
    bool verbose = false;
    bool dryRun = false;
    bool force = false;
    bool help = false;
};

class ArgumentParser
{
public:
    static ParsedArgs parse(int argc, char* argv[]);
};
