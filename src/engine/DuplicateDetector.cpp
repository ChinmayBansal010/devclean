#include "engine/DuplicateDetector.hpp"

#include <algorithm>
#include <map>
#include <system_error>

std::vector<DuplicateGroup> findDuplicateCandidates(const std::filesystem::path& root,
                                                     std::size_t minimumGroupSize)
{
    std::map<uint64_t, std::vector<std::filesystem::path>> bySize;
    std::error_code ec;
    if (!std::filesystem::is_directory(root, ec))
        return {};

    std::filesystem::recursive_directory_iterator it(root, std::filesystem::directory_options::skip_permission_denied, ec);
    const std::filesystem::recursive_directory_iterator end;
    while (it != end)
    {
        if (!ec && it->is_regular_file(ec))
        {
            const uint64_t size = static_cast<uint64_t>(it->file_size(ec));
            if (!ec && size > 0)
                bySize[size].push_back(it->path());
        }
        ec.clear();
        it.increment(ec);
        ec.clear();
    }

    std::vector<DuplicateGroup> groups;
    for (auto& [size, files] : bySize)
    {
        if (files.size() < minimumGroupSize)
            continue;
        DuplicateGroup group;
        group.size = size;
        group.files = std::move(files);
        group.reclaimableBytes = size * static_cast<uint64_t>(group.files.size() - 1);
        groups.push_back(std::move(group));
    }

    std::stable_sort(groups.begin(), groups.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.reclaimableBytes != rhs.reclaimableBytes)
            return lhs.reclaimableBytes > rhs.reclaimableBytes;
        return lhs.files.size() > rhs.files.size();
    });
    return groups;
}
