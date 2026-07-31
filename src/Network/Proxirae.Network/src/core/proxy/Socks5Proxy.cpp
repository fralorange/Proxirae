#include <stdexcept>
#include <format>
#include <vector>

#include "platform/sock.h"
#include "platform/inet.h"
#include "core/proxy/Socks5Proxy.h"

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
			m_logger.LogWarning("Failed to connect to the SOCKS5 proxy. Error=Proxy connection already established!");

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
			m_logger.LogWarning("Failed to the disconnect the SOCKS5 proxy. Error=Proxy connection is not established!");

			return;
		}

		std::string message;

		message = std::format("Disconnecting from the SOCKS5 proxy: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		shutdown(m_proxy, ShutdownBoth);
		CloseSocket(m_proxy);

		m_proxy = InvalidNativeSocket;
		m_connected = false;

		message = std::format("Disconnected from the SOCKS5 proxy: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);
	}

	void Socks5Proxy::Send(std::span<const char> buffer, std::function<void(const IoResult&)> callback)
	{
		if (!m_connected || m_proxy == InvalidNativeSocket) {
			m_logger.LogError("Failed to send data through the SOCKS5 proxy. Error=Proxy is not connected");
			callback(IoResult{ false, 0, SocketNotConnected });
			
			return;
		}

		m_driver.AsyncWrite(m_proxy, buffer, callback);
	}

	void Socks5Proxy::Recv(std::span<char> buffer, std::function<void(const IoResult&)> callback)
	{
		if (!m_connected || m_proxy == InvalidNativeSocket) {
			m_logger.LogError("Failed to receive data from the SOCKS5 proxy. Error=Proxy is not connected");
			callback(IoResult{ false, 0, SocketNotConnected });

			return;
		}

		m_driver.AsyncRead(m_proxy, buffer, callback);
	}

	NativeSocket Socks5Proxy::ConnectToProxy()
	{
		NativeSocket sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sock == InvalidNativeSocket) {
			m_logger.LogCritical("Failed to create SOCKS5 proxy socket.");

			return InvalidNativeSocket;
		}

		struct sockaddr_in proxyAddr{};

		inet_pton(AF_INET, m_address.c_str(), &proxyAddr.sin_addr.s_addr);
		proxyAddr.sin_port = htons(m_port);
		proxyAddr.sin_family = AF_INET;

		std::string message = std::format("Connecting to the SOCKS5 proxy: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		if (connect(sock, reinterpret_cast<struct sockaddr*>(&proxyAddr), sizeof(proxyAddr)) == SocketError) {
			m_logger.LogCritical("Failed to connect to the SOCKS5 proxy.");
			CloseSocket(sock);

			return InvalidNativeSocket;
		}

		return sock;
	}

	bool Socks5Proxy::PerformHandshake(NativeSocket sock)
	{
		bool requiresAuth = !m_username.empty() && !m_password.empty();
		
		char greeting[3] = { 0x05, 0x01, requiresAuth ? 0x02 : 0x00 };

		std::string message;

		message = std::format("Initiating the SOCKS5 proxy handshake: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		if (!SendExact(sock, greeting)) {
			m_logger.LogCritical("Failed to initiate the SOCKS5 proxy handshake.");
			CloseSocket(sock);

			return false;
		}

		char response[2]{};
		if (!RecvExact(sock, response)) {
			m_logger.LogCritical("Failed to complete the SOCKS5 proxy handshake.");
			CloseSocket(sock);

			return false;
		}

		if (!requiresAuth) {
			if (response[0] != 0x05 || response[1] != 0x00) {
				m_logger.LogCritical("Failed to negotiate 'No Auth' method with the SOCKS5 proxy.");
				CloseSocket(sock);

				return false;
			}
		}
		else {
			if (response[0] != 0x05 || response[1] != 0x02) {
				m_logger.LogCritical("Failed to negotiate 'Username/Password' method with the SOCKS5 proxy.");
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
				m_logger.LogCritical("Failed to subnegotiate authentication.");
				CloseSocket(sock);

				return false;
			}

			char authResp[2]{};
			if (!RecvExact(sock, authResp)) {
				m_logger.LogCritical("Failed to complete subnegotiation.");
				CloseSocket(sock);

				return false;
			}

			if (authResp[1] != 0x00) {
				m_logger.LogCritical("Rejected SOCKS5 Authentication. Error=Wrong username/password.");
				CloseSocket(sock);

				return false;
			}
		}

		message = std::format("Finished the SOCKS5 proxy handshake: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		return true;
	}

	bool Socks5Proxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
	{
		std::vector<char> connReq = {
			0x05, // VERSION 5
			0x01, // CMD: 0x01 (CONNECT)
			0x00, // Reserved: 0x00
			0x01, // Address Type: 0x01 (IPv4)
		};

		struct in_addr ipv4Addr{};
		if (inet_pton(AF_INET, targetAddress.data(), &ipv4Addr) != 1) {
			return false; // NOT IP
		}

		const char* ipBytes = reinterpret_cast<const char*>(&ipv4Addr.s_addr);

		connReq.insert(connReq.end(), ipBytes, ipBytes + 4);
		connReq.push_back((targetPort >> 8) & 0xFF);
		connReq.push_back(targetPort & 0xFF);

		if (!SendExact(sock, connReq)) {
			m_logger.LogCritical("Failed to initiate the SOCKS5 proxy CONNECT request.");
			CloseSocket(sock);

			return false;
		}

		char connHeader[4]{};
		if (!RecvExact(sock, connHeader)) {
			m_logger.LogCritical("Failed to receive connection header from the SOCKS5 proxy.");
			CloseSocket(sock);

			return false;
		}

		std::string message;

		if (connHeader[1] != 0x00) {
			message = std::format("Rejected SOCKS5 connection to target. Status={}", static_cast<int>(connHeader[1]));
			m_logger.LogCritical(message);
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
			std::vector<char> domainAddr(len + 2);
			RecvExact(sock, domainAddr);
		}

		message = std::format("Established connection to target via SOCKS5 proxy: Target={}:{}", targetAddress, targetPort);
		m_logger.LogInfo(message);

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