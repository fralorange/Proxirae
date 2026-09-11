#include <format>

#include "proxification/Socks5ProxyBase.h"
#include "environment/sock.h"

namespace Proxirae {
	Socks5ProxyBase::Socks5ProxyBase(std::string_view address, std::uint16_t port, IAsyncDriver& driver, ILogger& logger)
		: m_address(address), m_port(port), m_driver(driver), m_logger(logger) { }

	Socks5ProxyBase::Socks5ProxyBase(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, ILogger& logger)
		: Socks5ProxyBase(address, port, driver, logger) 
	{
		m_username = username;
		m_password = password;
	}

	NativeSocket Socks5ProxyBase::ConnectToProxy()
	{
		NativeSocket sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sock == InvalidNativeSocket) {
			m_logger.LogError(std::format("[SOCKS5] Socket creation failed: error {}", GetSocketError()));

			return InvalidNativeSocket;
		}

		struct sockaddr_in proxyAddr {};

		inet_pton(AF_INET, m_address.c_str(), &proxyAddr.sin_addr.s_addr);
		proxyAddr.sin_port = htons(m_port);
		proxyAddr.sin_family = AF_INET;

		m_logger.LogDebug(std::format("[SOCKS5] Connecting to {}:{}", m_address, m_port));

		if (connect(sock, reinterpret_cast<struct sockaddr*>(&proxyAddr), sizeof(proxyAddr)) == SocketError) {
			m_logger.LogError(std::format("[SOCKS5] Connection failed to {}:{}", m_address, m_port));
			CloseSocket(sock);

			return InvalidNativeSocket;
		}

		return sock;
	}

	bool Socks5ProxyBase::PerformHandshake(NativeSocket sock) {
		bool requiresAuth = !m_username.empty() && !m_password.empty();

		char greeting[3] = { 0x05, 0x01, requiresAuth ? 0x02 : 0x00 };

		m_logger.LogDebug(std::format("[SOCKS5] Initiating handshake with {}:{}", m_address, m_port));

		if (!SendExact(sock, greeting)) {
			m_logger.LogError(std::format("[SOCKS5] Handshake greeting send failed ({}:{})", m_address, m_port));
			CloseSocket(sock);

			return false;
		}

		char response[2]{};
		if (!RecvExact(sock, response)) {
			m_logger.LogError(std::format("[SOCKS5] Handshake response read failed ({}:{})", m_address, m_port));
			CloseSocket(sock);

			return false;
		}

		if (!requiresAuth) {
			if (response[0] != 0x05 || response[1] != 0x00) {
				m_logger.LogError(std::format("[SOCKS5] Negotiation 'No Auth' rejected by {}:{}", m_address, m_port));
				CloseSocket(sock);

				return false;
			}
		}
		else {
			if (response[0] != 0x05 || response[1] != 0x02) {
				m_logger.LogError(std::format("[SOCKS5] Negotiation 'User/Password' rejected by {}:{}", m_address, m_port));
				CloseSocket(sock);

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
				CloseSocket(sock);

				return false;
			}

			char authResp[2]{};
			if (!RecvExact(sock, authResp)) {
				m_logger.LogError(std::format("[SOCKS5] Auth response read failed ({}:{})", m_address, m_port));
				CloseSocket(sock);

				return false;
			}

			if (authResp[1] != 0x00) {
				m_logger.LogError(std::format("[SOCKS5] Authentication failed: invalid credentials ({}:{})", m_address, m_port));
				CloseSocket(sock);

				return false;
			}
		}

		m_logger.LogDebug(std::format("[SOCKS5] Handshake successful with {}:{}", m_address, m_port));

		return true;
	}

	bool Socks5ProxyBase::SendExact(NativeSocket sock, std::span<const char> buffer)
	{
		std::size_t totalSent = 0;
		std::size_t length = buffer.size();

		while (totalSent < length) {
			int bytesSent = send(sock, buffer.data() + totalSent, static_cast<int>(length - totalSent), 0);

			if (bytesSent == SocketError) {
				return false;
			}

			totalSent += bytesSent;
		}

		return true;
	}

	bool Socks5ProxyBase::RecvExact(NativeSocket sock, std::span<char> buffer)
	{
		std::size_t totalReceived = 0;
		std::size_t length = buffer.size();

		while (totalReceived < length) {
			int bytesReceived = recv(sock, buffer.data() + totalReceived, static_cast<int>(length - totalReceived), 0);

			if (bytesReceived <= 0) {
				return false;
			}

			totalReceived += bytesReceived;
		}

		return true;
	}
}