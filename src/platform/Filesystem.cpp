#include "platform/Filesystem.hpp"

#include <algorithm>
#include <cstdlib>
#include <set>
#include <system_error>

namespace fs = std::filesystem;

namespace {

std::string normalizePathString(std::string value)
{
#ifdef _WIN32
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    std::replace(value.begin(), value.end(), '\\', '/');
#endif
    return value;
}

bool hasProtectedPrefix(const std::string& path, const std::string& protectedPath)
{
    if (path == protectedPath)
        return true;
    if (path.size() <= protectedPath.size())
        return false;
    return path.rfind(protectedPath + "/", 0) == 0;
}

} // namespace

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
        if (entry.is_symlink(entryEc) && !entryEc)
        {
            std::error_code targetEc;
            const auto targetStatus = fs::status(entry.path(), targetEc);
            if (targetEc || !fs::exists(targetStatus))
            {
                ++summary.brokenSymlinks;
                continue;
            }
        }

        if (entry.is_directory(entryEc) && !entryEc)
        {
            ++summary.directories;
        }
        else if (entry.exists(entryEc) && !entryEc)
        {
            ++summary.files;
            std::error_code sizeEc;
            const auto size = entry.file_size(sizeEc);
            if (!sizeEc)
            {
                summary.bytes += size;
                const auto extension = entry.path().extension().string();
                summary.fileTypeBytes[extension.empty() ? "<no extension>" : extension] += size;
            }
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

bool Filesystem::removeDirectory(const fs::path& path, std::string* error)
{
    std::error_code ec;
    if (path.empty())
    {
        if (error != nullptr)
            *error = "empty path";
        return false;
    }

    if (isProtectedPath(path))
    {
        if (error != nullptr)
            *error = "refusing to delete protected directory";
        return false;
    }

    if (!exists(path))
        return true;

    std::error_code symlinkEc;
    if (fs::is_symlink(path, symlinkEc) && !symlinkEc)
    {
        if (error != nullptr)
            *error = "refusing to delete top-level symlink path";
        return false;
    }

    const auto removed = fs::remove_all(path, ec);
    if (ec && error != nullptr)
        *error = ec.message();
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
        if (entry.exists(entryEc) && !entryEc)
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

fs::path Filesystem::weaklyCanonical(const fs::path& path)
{
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(path, ec);
    return ec ? path.lexically_normal() : normalized;
}

bool Filesystem::isProtectedPath(const fs::path& path)
{
    if (path.empty())
        return true;

    const fs::path canonical = weaklyCanonical(path);
    const std::string normalized = normalizePathString(canonical.string());

    if (normalized.empty())
        return true;

#ifdef _WIN32
    static const std::set<std::string> staticProtected = {
        "c:/",
        "c:",
        "c:/windows",
        "c:/program files",
        "c:/program files (x86)"
    };
    if (staticProtected.find(normalized) != staticProtected.end())
        return true;
    if (normalized.size() <= 3 && normalized.size() >= 2 && std::isalpha(static_cast<unsigned char>(normalized[0])) && normalized[1] == ':')
        return true;
#else
    if (normalized == "/")
        return true;
    static const std::set<std::string> staticProtected = {
        "/bin",
        "/boot",
        "/dev",
        "/etc",
        "/home",
        "/lib",
        "/lib64",
        "/opt",
        "/proc",
        "/root",
        "/sbin",
        "/srv",
        "/sys",
        "/usr",
        "/var"
    };
    if (staticProtected.find(normalized) != staticProtected.end())
        return true;
#endif

    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        const std::string homePath = normalizePathString(weaklyCanonical(home).string());
        if (!homePath.empty() && hasProtectedPrefix(normalized, homePath))
            return normalized == homePath;
    }

#ifdef _WIN32
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile != nullptr)
    {
        const std::string profilePath = normalizePathString(weaklyCanonical(userProfile).string());
        if (!profilePath.empty() && hasProtectedPrefix(normalized, profilePath))
            return normalized == profilePath;
    }
#endif

    return false;
}
