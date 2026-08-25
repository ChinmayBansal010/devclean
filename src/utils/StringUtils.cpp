#include "utils/StringUtils.hpp"

#include <algorithm>
#include <cctype>

namespace StringUtils {

std::string lower(std::string_view value)
{
    std::string result(value);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

bool startsWith(std::string_view value, std::string_view prefix)
{
    return value.size() >= prefix.size() && value.substr(0, prefix.size()) == prefix;
}

bool endsWith(std::string_view value, std::string_view suffix)
{
    return value.size() >= suffix.size() && value.substr(value.size() - suffix.size()) == suffix;
}

bool equalsIgnoreCase(std::string_view lhs, std::string_view rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    for (std::size_t i = 0; i < lhs.size(); ++i)
    {
        const auto left = static_cast<unsigned char>(lhs[i]);
        const auto right = static_cast<unsigned char>(rhs[i]);
        if (std::tolower(left) != std::tolower(right))
            return false;
    }
    return true;
}

bool endsWithIgnoreCase(std::string_view value, std::string_view suffix)
{
    return value.size() >= suffix.size() &&
        equalsIgnoreCase(value.substr(value.size() - suffix.size()), suffix);
}

std::string trim(std::string_view value)
{
    std::size_t first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])))
        ++first;

    std::size_t last = value.size();
    while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1])))
        --last;

    return std::string(value.substr(first, last - first));
}

}
