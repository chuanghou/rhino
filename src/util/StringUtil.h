#pragma once
#include <algorithm>
#include <vector>
#include <string>

namespace rhino {

__attribute__((unused)) bool isBlank(const std::string &val) {
    bool blank = false;
    if (std::all_of(val.begin(), val.end(),
                    [](const char &c) { return std::isspace(c); })) {
        blank = true;
    }
    return blank;
}

__attribute__((unused)) std::vector<std::string> spiltBy(const std::string &val) {
    std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
        vector<string> tokens;
        size_t start = 0;
        size_t end = s.find(delimiter);

        while (end != string::npos) {
            tokens.push_back(s.substr(start, end - start));
            start = end + delimiter.length();
            end = s.find(delimiter, start);
        }

        tokens.push_back(s.substr(start));

        return tokens;
    }
}
