#include <stdexcept>
#include <format>
#include <vector>

#include "environment/sock.h"
#include "environment/inet.h"
#include "proxification/stream/TcpSocks5Proxy.h"

namespace Proxirae {
	TcpSocks5Proxy::TcpSocks5Proxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
		: Socks5ProxyBase(address, port, driver, logger), m_adapter(adapter) { }

	TcpSocks5Proxy::TcpSocks5Proxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
		: Socks5ProxyBase(address, port, username, password, driver, logger), m_adapter(adapter) { }

	TcpSocks5Proxy::~TcpSocks5Proxy() {
		Disconnect();
	}

	bool TcpSocks5Proxy::Connect(std::string_view targetAddress, std::uint16_t targetPort) {
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

	void TcpSocks5Proxy::Disconnect() {
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

	void TcpSocks5Proxy::Send(std::span<const std::byte> buffer, std::function<void(const IoResult&)> callback)
	{
		if (!m_connected || m_proxy == InvalidNativeSocket) {
			m_logger.LogError(std::format("[SOCKS5] Send failed: proxy not connected ({}:{})", m_address, m_port));
			callback(IoResult{ false, 0, SocketNotConnected });
			
			return;
		}

		m_adapter.AsyncWrite(m_proxy, buffer, callback);
	}

	void TcpSocks5Proxy::Recv(std::span<std::byte> buffer, std::function<void(const IoResult&)> callback)
	{
		if (!m_connected || m_proxy == InvalidNativeSocket) {
			m_logger.LogError(std::format("[SOCKS5] Recv failed: proxy not connected ({}:{})", m_address, m_port));
			callback(IoResult{ false, 0, SocketNotConnected });

			return;
		}

		m_adapter.AsyncRead(m_proxy, buffer, callback);
	}

	bool TcpSocks5Proxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
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
}