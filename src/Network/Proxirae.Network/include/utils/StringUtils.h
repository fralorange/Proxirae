#pragma once
#include <string_view>

namespace Proxirae {
    class StringUtils {
    public:
        static std::string_view Trim(std::string_view str);
        static std::string_view TrimQuotes(std::string_view str);
        static bool WildcardMatch(std::string_view text, std::string_view pattern);
    };
}