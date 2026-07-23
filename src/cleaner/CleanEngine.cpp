#include "cleaner/CleanEngine.hpp"

#include "platform/Filesystem.hpp"

#include <filesystem>

namespace fs = std::filesystem;

bool CleanEngine::removeDirectory(const fs::path& path)
{
    return Filesystem::removeDirectory(path);
}