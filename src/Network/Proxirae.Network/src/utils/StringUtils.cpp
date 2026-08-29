#include <ranges>
#include <ctype.h>

#include "utils/StringUtils.h"

namespace Proxirae::StringUtils {
    std::string_view Trim(std::string_view str) {
        const auto start = str.find_first_not_of(" \t\r\n");
        if (start == std::string_view::npos) return {};
        const auto end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    std::string_view TrimQuotes(std::string_view str) {
        if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
            return str.substr(1, str.size() - 2);
        }
        return str;
    }

    bool WildcardMatch(std::string_view text, std::string_view pattern) {
        const char* t = text.data();
        const char* p = pattern.data();
        const char* textEnd = t + text.size();
        const char* patEnd = p + pattern.size();

        const char* star = nullptr;
        const char* t_match = nullptr;

        while (t < textEnd) {
            if (p < patEnd && (*p == '?' ||
                tolower(static_cast<unsigned char>(*p)) == tolower(static_cast<unsigned char>(*t)))) {
                ++t;
                ++p;
            }
            else if (p < patEnd && *p == '*') {
                star = p++;
                t_match = t;
            }
            else if (star) {
                p = star + 1;
                t = ++t_match;
            }
            else {
                return false;
            }
        }
        while (p < patEnd && *p == '*') ++p;
        return p == patEnd;
    }

    std::string ToUTF8(const std::filesystem::path& path)
    {
        auto u8 = path.u8string();
        return { u8.begin(), u8.end() };
    }
}