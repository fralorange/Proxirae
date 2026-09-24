#include <Windows.h>
#include <wincrypt.h>
#include <dpapi.h>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "Crypt32.lib")

#include "protection/win/WinProtector.h"

namespace Proxirae {
	bool WinProtector::TryUnprotect(std::string_view cipherText, std::string& plainText)
	{
		plainText.clear();

		if (cipherText.empty()) {
			return true;
		}

		try {
			plainText = Unprotect(cipherText);

			return true;
		}
		catch (...) {
			return false;
		}
	}

	std::string WinProtector::Unprotect(std::string_view cipherText) const
	{
		DWORD binarySize = 0;
		if (!CryptStringToBinaryA(cipherText.data(), static_cast<DWORD>(cipherText.size()),
			CRYPT_STRING_BASE64, nullptr, &binarySize, nullptr, nullptr)) {
			throw std::runtime_error("Invalid Base64 format.");
		}

		std::vector<BYTE> encryptedBytes(binarySize);
		if (!CryptStringToBinaryA(cipherText.data(), static_cast<DWORD>(cipherText.size()),
			CRYPT_STRING_BASE64, encryptedBytes.data(), &binarySize, nullptr, nullptr)) {
			throw std::runtime_error("Failed to decode Base64 string.");
		}

		DATA_BLOB inBlob{};
		inBlob.pbData = encryptedBytes.data();
		inBlob.cbData = static_cast<DWORD>(encryptedBytes.size());

		DATA_BLOB outBlob{};

		if (!CryptUnprotectData(&inBlob, nullptr, nullptr, nullptr, nullptr, 0, &outBlob)) {
			throw std::runtime_error("CryptUnprotectData failed.");
		}

		std::string plainText(reinterpret_cast<char*>(outBlob.pbData), outBlob.cbData);

		LocalFree(outBlob.pbData);

		return plainText;
	}
}