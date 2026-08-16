#include <cassert>
#include <filesystem>

int main()
{
    const auto marker = std::filesystem::temp_directory_path() / "devclean-dry-run-marker";
    std::filesystem::remove(marker);
    assert(!std::filesystem::exists(marker));
    return 0;
}
