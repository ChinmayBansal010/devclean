#pragma once

#include <string>
#include <string_view>

namespace StringUtils {
std::string lower(std::string_view value);
bool startsWith(std::string_view value, std::string_view prefix);
bool endsWith(std::string_view value, std::string_view suffix);
bool equalsIgnoreCase(std::string_view lhs, std::string_view rhs);
std::string trim(std::string_view value);
}
