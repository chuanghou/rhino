#pragma once

namespace rhino {


          __attribute__((unused)) static bool isBlank(const std::string &val) {
            bool blank = false;
            if (std::all_of(val.begin(), val.end(), [](const char &c) { return std::isspace(c); })) {
                blank = true;
            }
            return blank;
        }


         __attribute__((unused)) static std::vector spiltBy(const std::string &val) {
            vector<string> split(const string& s, const string& delimiter) {
    vector<string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    
    while (end != string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    
    // 添加最后一个部分
    tokens.push_back(s.substr(start));
    
    return tokens;
}
}

