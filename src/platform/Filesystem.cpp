#include "platform/Filesystem.hpp"

#include <system_error>

namespace fs = std::filesystem;

bool Filesystem::exists(const fs::path& path)
{
    std::error_code ec;
    return fs::exists(path, ec) && !ec;
}

Filesystem::DirectorySummary Filesystem::inspectDirectory(const fs::path& path)
{
    DirectorySummary summary;
    summary.exists = exists(path);
    if (!summary.exists)
        return summary;

    std::error_code ec;
    for (fs::recursive_directory_iterator it(path, fs::directory_options::skip_permission_denied, ec), end;
         it != end;
         it.increment(ec))
    {
        if (ec)
        {
            summary.success = false;
            summary.error = ec.message();
            break;
        }

        const auto& entry = *it;
        std::error_code entryEc;
        if (entry.is_directory(entryEc) && !entryEc)
        {
            ++summary.directories;
        }
        else if (entry.is_regular_file(entryEc) && !entryEc)
        {
            ++summary.files;
            std::error_code sizeEc;
            const auto size = entry.file_size(sizeEc);
            if (!sizeEc)
                summary.bytes += size;
        }
    }

    if (!summary.success && summary.error.empty())
        summary.error = "unreadable directory";

    return summary;
}

uint64_t Filesystem::directorySize(const fs::path& dir)
{
    return inspectDirectory(dir).bytes;
}


bool Filesystem::removeDirectory(const fs::path& path)
{
    std::error_code ec;
    if (!exists(path))
        return true;

    const auto removed = fs::remove_all(path, ec);
    return !ec && removed > 0;
}

fs::file_time_type Filesystem::lastModified(const fs::path& path)
{
    std::error_code ec;
    if (!exists(path))
        return {};
    return fs::last_write_time(path, ec);
}

uint64_t Filesystem::fileCount(const fs::path& path)
{
    uint64_t count = 0;
    if (!exists(path))
        return 0;

    std::error_code ec;
    for (fs::recursive_directory_iterator it(path, fs::directory_options::skip_permission_denied, ec), end;
         it != end;
         it.increment(ec))
    {
        if (ec)
            break;

        const auto& entry = *it;
        std::error_code entryEc;
        if (entry.is_regular_file(entryEc) && !entryEc)
            ++count;
    }

    return count;
}

uint64_t Filesystem::directoryCount(const fs::path& path)
{
    uint64_t count = 0;
    if (!exists(path))
        return 0;

    std::error_code ec;
    for (fs::recursive_directory_iterator it(path, fs::directory_options::skip_permission_denied, ec), end;
         it != end;
         it.increment(ec))
    {
        if (ec)
            break;

        const auto& entry = *it;
        std::error_code entryEc;
        if (entry.is_directory(entryEc) && !entryEc)
            ++count;
    }

    return count;
}