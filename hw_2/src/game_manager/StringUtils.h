#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <sstream>

namespace StringUtils {
    // Trims leading and trailing whitespace from a string
    inline std::string trim(const std::string &s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            ++start;
        }
        auto end = s.end();
        do {
            --end;
        } while (std::distance(start, end) > 0 && std::isspace(*end));

        return std::string(start, end + 1);
    }

    // Splits a string by a delimiter and trims each part
    inline std::vector<std::string> split_and_trim(const std::string &input, char delimiter) {
        std::vector<std::string> result;
        std::istringstream stream(input);
        std::string token;

        while (std::getline(stream, token, delimiter)) {
            result.push_back(trim(token));
        }

        return result;
    }
} // namespace StringUtils

#endif // STRING_UTILS_H
