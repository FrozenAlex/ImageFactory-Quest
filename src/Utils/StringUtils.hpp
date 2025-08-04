#pragma once

#include <string>
#include <vector>

namespace StringUtils {
    std::string removeTrailingZeros(int s);
    std::vector<std::string> split(std::string s, char c);
}