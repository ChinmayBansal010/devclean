#include "engine/PathSafety.hpp"

#include "platform/Filesystem.hpp"

#include <system_error>

PathSafetyResult inspectPathSafety(const std::filesystem::path& path)
{
    PathSafetyResult result;
    std::error_code ec;
    result.exists = std::filesystem::exists(path, ec);
    if (ec)
    {
        result.reason = ec.message();
        return result;
    }

    result.symlink = std::filesystem::is_symlink(path, ec);
    if (ec)
    {
        result.reason = ec.message();
        return result;
    }

    result.protectedLocation = Filesystem::isProtectedPath(path);
    if (result.protectedLocation)
    {
        result.reason = "path is protected";
        return result;
    }

    if (result.symlink)
    {
        result.reason = "symbolic links require explicit review";
        return result;
    }

    if (!result.exists)
    {
        result.reason = "path does not exist";
        return result;
    }

    result.safe = true;
    result.reason = "path is a regular non-protected filesystem location";
    return result;
}

bool isPathInside(const std::filesystem::path& child, const std::filesystem::path& parent)
{
    std::error_code ec;
    const auto childCanonical = std::filesystem::weakly_canonical(child, ec);
    if (ec)
        return false;
    const auto parentCanonical = std::filesystem::weakly_canonical(parent, ec);
    if (ec)
        return false;

    auto childIt = childCanonical.begin();
    auto parentIt = parentCanonical.begin();
    for (; parentIt != parentCanonical.end(); ++parentIt, ++childIt)
    {
        if (childIt == childCanonical.end() || *childIt != *parentIt)
            return false;
    }
    return true;
}
