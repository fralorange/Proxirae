#pragma once

#include "protection/IProtector.h"

namespace Proxirae {
	class WinProtector : public IProtector {
	public:
		bool TryUnprotect(std::string_view cipherText, std::string& plainText) override;

	private:
		std::string Unprotect(std::string_view cipherText) const;
	};
}