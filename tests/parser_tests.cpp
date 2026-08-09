#include "core/ArgumentParser.hpp"

#include <cassert>

int main()
{
    {
        char* argv[] = {
            const_cast<char*>("devclean"),
            const_cast<char*>("scan"),
            const_cast<char*>("--category"),
            const_cast<char*>("python"),
            const_cast<char*>("--json")
        };

        const ParsedArgs args = ArgumentParser::parse(5, argv);
        assert(args.command == "scan");
        assert(args.category == "python");
        assert(args.json);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"),
            const_cast<char*>("scan"),
            const_cast<char*>("--category"),
            const_cast<char*>("--json"),
            const_cast<char*>("--reverse")
        };

        const ParsedArgs args = ArgumentParser::parse(5, argv);
        assert(args.command == "scan");
        assert(args.category.empty());
        assert(args.json);
        assert(args.reverse);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"),
            const_cast<char*>("analyze"),
            const_cast<char*>("--report"),
            const_cast<char*>("json"),
            const_cast<char*>("project-a"),
            const_cast<char*>("project-b")
        };

        const ParsedArgs args = ArgumentParser::parse(6, argv);
        assert(args.command == "analyze");
        assert(args.reportFormat == "json");
        assert(args.targets.size() == 2);
        assert(args.targets[0] == "project-a");
        assert(args.targets[1] == "project-b");
    }

    return 0;
}
