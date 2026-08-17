#include "engine/SizeEstimator.hpp"

#include <system_error>

SizeEstimate estimateDirectorySize(const std::filesystem::path& root)
{
    SizeEstimate estimate;
    std::error_code ec;
    if (!std::filesystem::is_directory(root, ec))
    {
        estimate.complete = false;
        return estimate;
    }

    std::filesystem::recursive_directory_iterator it(root, std::filesystem::directory_options::skip_permission_denied, ec);
    const std::filesystem::recursive_directory_iterator end;
    while (it != end)
    {
        const auto status = it->symlink_status(ec);
        if (ec)
        {
            estimate.complete = false;
            ec.clear();
        }
        else if (std::filesystem::is_directory(status))
        {
            ++estimate.directories;
        }
        else if (std::filesystem::is_regular_file(status))
        {
            const auto size = it->file_size(ec);
            if (ec)
            {
                estimate.complete = false;
                ec.clear();
            }
            else
            {
                estimate.bytes += static_cast<uint64_t>(size);
                ++estimate.files;
            }
        }

        it.increment(ec);
        if (ec)
        {
            estimate.complete = false;
            ec.clear();
        }
    }
    return estimate;
}
