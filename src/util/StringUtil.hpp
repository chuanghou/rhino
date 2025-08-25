#pragma once
#include <algorithm>
#include <string>
#include <vector>

namespace rhino {

inline bool isBlank(const std::string &val) {
    bool blank = false;
    if (std::all_of(val.begin(), val.end(),
                    [](const char &c) { return std::isspace(c); })) {
        blank = true;
    }
    return blank;
}

inline std::vector<std::string> split(const std::string &s,
                               const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }

    tokens.push_back(s.substr(start));

    return tokens;
}
} // namespace rhino
