#include <cassert>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 2)
        return 0;

    std::ifstream input(argv[1]);
    if (!input)
        return 0;

    const std::string content((std::istreambuf_iterator<char>(input)), {});
    assert(!content.empty());
    assert(content.front() == '{' || content.front() == '[');
    return 0;
}
