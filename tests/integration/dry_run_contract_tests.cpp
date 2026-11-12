#include <cassert>
#include <filesystem>

int main()
{
    const auto marker = std::filesystem::temp_directory_path() / "devclean-dry-run-marker";
    std::filesystem::remove(marker);
    if (std::filesystem::exists(marker))
        return 1;
    return 0;
}
