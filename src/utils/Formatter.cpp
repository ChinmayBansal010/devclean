#include "utils/Formatter.hpp"

#include <iomanip>
#include <sstream>

namespace {
std::string formatValue(double size, const char* unit)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << size << ' ' << unit;
    return out.str();
}
} // namespace

std::string Formatter::formatBytes(uint64_t bytes)
{
    static constexpr const char* units[] = {"B", "KB", "MB", "GB", "TB"};

    double size = static_cast<double>(bytes);
    int unit = 0;

    while (size >= 1024.0 && unit < 4)
    {
        size /= 1024.0;
        ++unit;
    }

    return formatValue(size, units[unit]);
}

std::string Formatter::formatPath(const std::filesystem::path& path)
{
    return path.empty() ? "<unset>" : path.string();
}
