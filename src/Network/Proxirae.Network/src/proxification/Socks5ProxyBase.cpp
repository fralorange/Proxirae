#include <format>
#include <vector>

#include "proxification/Socks5ProxyBase.h"
#include "environment/sock.h"

namespace Proxirae {
	bool Socks5ProxyBase::PerformHandshake(NativeSocket sock) {
		bool requiresAuth = !m_username.empty() && !m_password.empty();

		char greeting[3] = { 0x05, 0x01, requiresAuth ? 0x02 : 0x00 };

		m_logger.LogDebug(std::format("[SOCKS5] Initiating handshake with {}:{}", m_address, m_port));

		if (!SendExact(sock, greeting)) {
			m_logger.LogError(std::format("[SOCKS5] Handshake greeting send failed ({}:{})", m_address, m_port));

			return false;
		}

		char response[2]{};
		if (!RecvExact(sock, response)) {
			m_logger.LogError(std::format("[SOCKS5] Handshake response read failed ({}:{})", m_address, m_port));

			return false;
		}

		if (!requiresAuth) {
			if (response[0] != 0x05 || response[1] != 0x00) {
				m_logger.LogError(std::format("[SOCKS5] Negotiation 'No Auth' rejected by {}:{}", m_address, m_port));

				return false;
			}
		}
		else {
			if (response[0] != 0x05 || response[1] != 0x02) {
				m_logger.LogError(std::format("[SOCKS5] Negotiation 'User/Password' rejected by {}:{}", m_address, m_port));

				return false;
			}

			std::vector<char> authReq{
				0x01 // AUTH VERSION
			};

			authReq.push_back(static_cast<char>(m_username.length()));
			authReq.insert(authReq.end(), m_username.begin(), m_username.end());
			authReq.push_back(static_cast<char>(m_password.length()));
			authReq.insert(authReq.end(), m_password.begin(), m_password.end());

			if (!SendExact(sock, authReq)) {
				m_logger.LogError(std::format("[SOCKS5] Auth credentials send failed ({}:{})", m_address, m_port));

				return false;
			}

			char authResp[2]{};
			if (!RecvExact(sock, authResp)) {
				m_logger.LogError(std::format("[SOCKS5] Auth response read failed ({}:{})", m_address, m_port));

				return false;
			}

			if (authResp[1] != 0x00) {
				m_logger.LogError(std::format("[SOCKS5] Authentication failed: invalid credentials ({}:{})", m_address, m_port));

				return false;
			}
		}

		m_logger.LogDebug(std::format("[SOCKS5] Handshake successful with {}:{}", m_address, m_port));

		return true;
	}
}