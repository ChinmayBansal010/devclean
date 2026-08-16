#include "core/ArgumentParser.hpp"

#include <cassert>
#include <cstdint>

int main()
{
    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("scan"),
            const_cast<char*>("--category"), const_cast<char*>("python"),
            const_cast<char*>("--json"), const_cast<char*>("--active-only")
        };
        const ParsedArgs args = ArgumentParser::parse(6, argv);
        assert(args.command == "scan");
        assert(args.category == "python");
        assert(args.json);
        assert(args.activeOnly);
        assert(args.targets.empty());
    }

    {
        char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("--version")};
        const ParsedArgs args = ArgumentParser::parse(2, argv);
        assert(args.version);
        assert(args.command.empty());
    }

    {
        char* argv[] = {const_cast<char*>("devclean"), const_cast<char*>("-V")};
        const ParsedArgs args = ArgumentParser::parse(2, argv);
        assert(args.version);
        assert(args.command.empty());
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("scan"),
            const_cast<char*>("--category"), const_cast<char*>("--json"),
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
            const_cast<char*>("devclean"), const_cast<char*>("analyze"),
            const_cast<char*>("--report"), const_cast<char*>("json"),
            const_cast<char*>("project-a"), const_cast<char*>("project-b")
        };
        const ParsedArgs args = ArgumentParser::parse(6, argv);
        assert(args.command == "analyze");
        assert(args.reportFormat == "json");
        assert(args.targets.size() == 2);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("scan"),
            const_cast<char*>("--min-size"), const_cast<char*>("512KB"),
            const_cast<char*>("--max-size"), const_cast<char*>("4GB")
        };
        const ParsedArgs args = ArgumentParser::parse(6, argv);
        assert(args.minSizeBytes == 512ULL * 1024ULL);
        assert(args.maxSizeBytes == 4ULL * 1024ULL * 1024ULL * 1024ULL);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("clean"),
            const_cast<char*>("--target"), const_cast<char*>("10GB"),
            const_cast<char*>("--stale"), const_cast<char*>("30d"),
            const_cast<char*>("--safe")
        };
        const ParsedArgs args = ArgumentParser::parse(7, argv);
        assert(args.command == "clean");
        assert(args.targetSizeBytes == 10ULL * 1024ULL * 1024ULL * 1024ULL);
        assert(args.staleSeconds == 30ULL * 24ULL * 60ULL * 60ULL);
        assert(args.safe);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("clean"),
            const_cast<char*>("--stale"), const_cast<char*>("12h")
        };
        const ParsedArgs args = ArgumentParser::parse(4, argv);
        assert(args.staleSeconds == 12ULL * 60ULL * 60ULL);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("scan"),
            const_cast<char*>("--max-size"), const_cast<char*>("18446744073709551615TB")
        };
        const ParsedArgs args = ArgumentParser::parse(4, argv);
        assert(args.maxSizeBytes == 0);
    }

    {
        char* argv[] = {
            const_cast<char*>("devclean"), const_cast<char*>("clean"),
            const_cast<char*>("--exclude"), const_cast<char*>("NPM"),
            const_cast<char*>("--sort"), const_cast<char*>("SIZE"),
            const_cast<char*>("python"), const_cast<char*>("cargo")
        };
        const ParsedArgs args = ArgumentParser::parse(8, argv);
        assert(args.command == "clean");
        assert(args.excludes.size() == 1);
        assert(args.excludes[0] == "npm");
        assert(args.sort == "size");
        assert(args.targets.size() == 2);
    }

    return 0;
}
