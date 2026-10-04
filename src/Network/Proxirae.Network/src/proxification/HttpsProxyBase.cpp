#include <format>
#include <vector>

#include "proxification/HttpsProxyBase.h"
#include "environment/sock.h"

namespace Proxirae {
	bool HttpsProxyBase::ReadHttpHeaders(NativeSocket sock, std::string& outHeaders)
	{
		outHeaders.clear();
        char c;

        while (true) {
            int bytesReceived = recv(sock, &c, 1, 0);
            if (bytesReceived <= 0) {
                m_logger.LogError(std::format("[HTTPS] Header read failed ({}:{})", m_address, m_port));
                return false;
            }

            outHeaders += c;

            if (outHeaders.size() >= 4 && outHeaders.ends_with("\r\n\r\n")) {
                break;
            }

            if (outHeaders.size() > 8192) {
                m_logger.LogError(std::format("[HTTPS] Header too large from proxy {}:{}", m_address, m_port));
                return false;
            }
        }
        return true;
	}

	std::string HttpsProxyBase::Base64Encode(std::string_view input)
	{
        static constexpr char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string ret;
        int i = 0, j = 0;
        unsigned char char_array_3[3], char_array_4[4];

        for (char c : input) {
            char_array_3[i++] = c;
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i > 0) {
            for (j = i; j < 3; j++) char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            for (j = 0; j < i + 1; j++) ret += base64_chars[char_array_4[j]];
            while (i++ < 3) ret += '=';
        }

        return ret;
	}
}