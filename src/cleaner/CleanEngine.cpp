#include "cleaner/CleanEngine.hpp"

#include "platform/Filesystem.hpp"

#include <filesystem>

namespace fs = std::filesystem;

CleanResult CleanEngine::removeDirectory(const fs::path& path)
{
    CleanResult result;
    result.success = Filesystem::removeDirectory(path, &result.error);
    return result;
}
