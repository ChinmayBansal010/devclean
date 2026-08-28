#include "engine/FileAgeIndex.hpp"

#include <algorithm>
#include <system_error>

namespace {

std::chrono::seconds fileAge(const std::filesystem::file_time_type& modified)
{
    const auto now = std::filesystem::file_time_type::clock::now();
    if (modified >= now)
        return std::chrono::seconds::zero();
    return std::chrono::duration_cast<std::chrono::seconds>(now - modified);
}

} // namespace

std::vector<FileAgeEntry> indexFileAges(const std::filesystem::path& root,
                                        std::chrono::seconds)
{
    std::vector<FileAgeEntry> entries;
    std::error_code ec;
    if (!std::filesystem::is_directory(root, ec))
        return entries;

    std::filesystem::recursive_directory_iterator it(root, std::filesystem::directory_options::skip_permission_denied, ec);
    const std::filesystem::recursive_directory_iterator end;
    while (it != end)
    {
        const auto status = it->symlink_status(ec);
        if (!ec && std::filesystem::is_regular_file(status))
        {
            const uint64_t bytes = static_cast<uint64_t>(it->file_size(ec));
            if (!ec)
            {
                const auto modified = it->last_write_time(ec);
                if (!ec)
                    entries.push_back({it->path(), bytes, fileAge(modified)});
            }
        }
        ec.clear();
        it.increment(ec);
        ec.clear();
    }

    std::stable_sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.age != rhs.age)
            return lhs.age > rhs.age;
        return lhs.path.string() < rhs.path.string();
    });

    return entries;
}

FileAgeSummary summarizeFileAges(const std::vector<FileAgeEntry>& entries,
                                 std::chrono::seconds staleAfter)
{
    FileAgeSummary summary;
    summary.fileCount = entries.size();
    for (const auto& entry : entries)
    {
        summary.totalBytes += entry.bytes;
        if (entry.age >= staleAfter)
        {
            summary.staleBytes += entry.bytes;
            ++summary.staleCount;
        }
    }
    return summary;
}
