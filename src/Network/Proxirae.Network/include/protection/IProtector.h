#pragma once

#include <string>
#include <string_view>

namespace Proxirae {
	class IProtector {
	public:
		virtual ~IProtector() = default;

		virtual bool TryUnprotect(std::string_view cipherText, std::string& plainText) = 0;
	};
}