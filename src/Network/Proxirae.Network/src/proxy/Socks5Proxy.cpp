#include <stdexcept>
#include <format>
#include <vector>

#include "platform/sock.h"
#include "platform/inet.h"
#include "proxy/Socks5Proxy.h"

namespace Proxirae {
	Socks5Proxy::Socks5Proxy(std::string_view address, std::uint16_t port, IIoDriver& driver, ILogger& logger)
		: m_address(address), m_port(port), m_driver(driver), m_logger(logger) {}

	Socks5Proxy::Socks5Proxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IIoDriver& driver, ILogger& logger)
		: Socks5Proxy(address, port, driver, logger)
	{
		m_username = username;
		m_password = password;
	}

	Socks5Proxy::~Socks5Proxy() {
		Disconnect();
	}

	bool Socks5Proxy::Connect(std::string_view targetAddress, std::uint16_t targetPort) {
		if (m_connected) {
			m_logger.LogWarning(std::format("[SOCKS5] Connect skipped: already connected to {}:{}", m_address, m_port));

			return false;
		}

		NativeSocket localSocket = ConnectToProxy();
		if (localSocket == InvalidNativeSocket) {
			return false;
		}

		if (!PerformHandshake(localSocket)) {
			CloseSocket(localSocket);

			return false;
		}

		if (!ConnectToTarget(localSocket, targetAddress, targetPort)) {
			CloseSocket(localSocket);

			return false;
		}

		if (!m_driver.Attach(localSocket)) {
			CloseSocket(localSocket);

			return false;
		}
		
		m_proxy = localSocket;
		m_connected = true;

		return true;
	}

	void Socks5Proxy::Disconnect() {
		if (!m_connected) {
			m_logger.LogWarning(std::format("[SOCKS5] Disconnect skipped: connection is not active ({}:{})", m_address, m_port));

			return;
		}

		m_logger.LogDebug(std::format("[SOCKS5] Closing connection to {}:{}", m_address, m_port));

		shutdown(m_proxy, ShutdownBoth);
		CloseSocket(m_proxy);

		m_proxy = InvalidNativeSocket;
		m_connected = false;

		m_logger.LogInfo(std::format("[SOCKS5] Disconnected from {}:{}", m_address, m_port));
	}

	void Socks5Proxy::Send(std::span<const std::byte> buffer, std::function<void(const IoResult&)> callback)
	{
		if (!m_connected || m_proxy == InvalidNativeSocket) {
			m_logger.LogError(std::format("[SOCKS5] Send failed: proxy not connected ({}:{})", m_address, m_port));
			callback(IoResult{ false, 0, SocketNotConnected });
			
			return;
		}

		m_driver.AsyncWrite(m_proxy, buffer, callback);
	}

	void Socks5Proxy::Recv(std::span<std::byte> buffer, std::function<void(const IoResult&)> callback)
	{
		if (!m_connected || m_proxy == InvalidNativeSocket) {
			m_logger.LogError(std::format("[SOCKS5] Recv failed: proxy not connected ({}:{})", m_address, m_port));
			callback(IoResult{ false, 0, SocketNotConnected });

			return;
		}

		m_driver.AsyncRead(m_proxy, buffer, callback);
	}

	NativeSocket Socks5Proxy::ConnectToProxy()
	{
		NativeSocket sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sock == InvalidNativeSocket) {
			m_logger.LogError(std::format("[SOCKS5] Socket creation failed: error {}", GetSocketError()));

			return InvalidNativeSocket;
		}

		struct sockaddr_in proxyAddr{};

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

	bool Socks5Proxy::PerformHandshake(NativeSocket sock)
	{
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

bool Socks5Proxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
	{
		std::vector<char> connReq;
		connReq.reserve(262); 
		connReq.push_back(0x05); // VERSION 5
		connReq.push_back(0x01); // CMD: 0x01 (CONNECT)
		connReq.push_back(0x00); // Reserved: 0x00

		std::string targetAddrStr(targetAddress);

		struct in_addr ipv4Addr{};
		struct in6_addr ipv6Addr{};

		if (inet_pton(AF_INET, targetAddrStr.c_str(), &ipv4Addr) == 1) {
			connReq.push_back(0x01); 
			const char* ipBytes = reinterpret_cast<const char*>(&ipv4Addr.s_addr);
			connReq.insert(connReq.end(), ipBytes, ipBytes + 4);
		}
		else if (inet_pton(AF_INET6, targetAddrStr.c_str(), &ipv6Addr) == 1) {
			connReq.push_back(0x04);
			const char* ipBytes = reinterpret_cast<const char*>(&ipv6Addr.s6_addr);
			connReq.insert(connReq.end(), ipBytes, ipBytes + 16);
		}
		else {
			if (targetAddrStr.empty() || targetAddrStr.length() > 255) {
				m_logger.LogError(std::format("[SOCKS5] Invalid domain name length: {}", targetAddrStr.length()));
				CloseSocket(sock);
				return false;
			}

			connReq.push_back(0x03); 
			connReq.push_back(static_cast<char>(targetAddrStr.length()));
			connReq.insert(connReq.end(), targetAddrStr.begin(), targetAddrStr.end());
		}

		connReq.push_back(static_cast<char>((targetPort >> 8) & 0xFF));
		connReq.push_back(static_cast<char>(targetPort & 0xFF));

		if (!SendExact(sock, connReq)) {
			m_logger.LogError(std::format("[SOCKS5] CONNECT request send failed for target {}:{}", targetAddress, targetPort));
			CloseSocket(sock);

			return false;
		}

		char connHeader[4]{};
		if (!RecvExact(sock, connHeader)) {
			m_logger.LogError(std::format("[SOCKS5] CONNECT response read failed for target {}:{}", targetAddress, targetPort));
			CloseSocket(sock);

			return false;
		}

		if (connHeader[1] != 0x00) {
			m_logger.LogError(std::format(
				"[SOCKS5] Proxy {}:{} rejected connection to {}:{}: status {}",
				m_address, m_port, targetAddress, targetPort, static_cast<int>(connHeader[1])
			));
			CloseSocket(sock);

			return false;
		}

		if (connHeader[3] == 0x01) { // IPv4
			char bindAddr[6]{}; 
			RecvExact(sock, bindAddr);
		}
		else if (connHeader[3] == 0x04) { // IPv6
			char bindAddr[18]{}; 
			RecvExact(sock, bindAddr);
		}
		else if (connHeader[3] == 0x03) { // Domain
			char len = 0;
			RecvExact(sock, std::span(&len, 1));
			std::vector<char> domainAddr(static_cast<unsigned char>(len) + 2);
			RecvExact(sock, domainAddr);
		}

		m_logger.LogInfo(std::format(
			"[SOCKS5] Tunnel established to {}:{} via {}:{}",
			targetAddress, targetPort, m_address, m_port
		));

		return true;
	}

	bool Socks5Proxy::SendExact(NativeSocket sock, std::span<const char> buffer)
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

	bool Socks5Proxy::RecvExact(NativeSocket sock, std::span<char> buffer)
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