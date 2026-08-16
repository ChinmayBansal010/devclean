#include "cleaner/CleanEngine.hpp"

#include "platform/Filesystem.hpp"

#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

CleanResult CleanEngine::removeDirectory(const fs::path& path)
{
    CleanResult result;
    result.success = Filesystem::removeDirectory(path, &result.error);
    return result;
}

CleanResult CleanEngine::removeStaleFiles(const fs::path& path, uint64_t staleSeconds, uint64_t targetBytes)
{
    CleanResult result;
    if (!fs::exists(path))
    {
        result.success = false;
        result.error = "Path does not exist.";
        return result;
    }

    const auto now = fs::file_time_type::clock::now();
    const auto threshold = std::chrono::seconds(staleSeconds);

    try
    {
        for (fs::recursive_directory_iterator it(path, fs::directory_options::skip_permission_denied), end; it != end; ++it)
        {
            if (!it->is_regular_file() || it->is_symlink())
                continue;

            const auto modified = it->last_write_time();
            if (now - modified < threshold)
                continue;

            const uint64_t bytes = static_cast<uint64_t>(it->file_size());
            if (targetBytes > 0 && bytes > targetBytes - result.bytesRemoved)
                continue;

            std::error_code ec;
            if (fs::remove(it->path(), ec))
            {
                result.bytesRemoved += bytes;
                ++result.filesRemoved;
            }
            else if (result.error.empty() && ec)
            {
                result.error = ec.message();
            }

            if (targetBytes > 0 && result.bytesRemoved >= targetBytes)
                break;
        }

        result.success = result.error.empty();
    }
    catch (const fs::filesystem_error& error)
    {
        result.error = error.what();
        result.success = false;
    }

    return result;
}
