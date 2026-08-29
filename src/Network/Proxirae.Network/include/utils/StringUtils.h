#pragma once
#include <string_view>
#include <string>
#include <filesystem>

namespace Proxirae::StringUtils {
    std::string_view Trim(std::string_view str);
    std::string_view TrimQuotes(std::string_view str);
    bool WildcardMatch(std::string_view text, std::string_view pattern);
    std::string ToUTF8(const std::filesystem::path& path);
}