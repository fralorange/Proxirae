#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdexcept>
#include <format>
#include <vector>

#include "core/Socks5Proxy.h"

namespace Proxirae {
	Socks5Proxy::Socks5Proxy(std::string_view address, UINT16 port, ILogger& logger)
		: m_address(address), m_port(port), m_logger(logger) {}

	Socks5Proxy::Socks5Proxy(std::string_view address, UINT16 port, std::string_view username, std::string_view password, ILogger& logger)
		: Socks5Proxy(address, port, logger)
	{
		m_username = username;
		m_password = password;
	}

	Socks5Proxy::~Socks5Proxy() {
		Disconnect();
	}

	bool Socks5Proxy::Connect(std::string_view targetAddress, UINT16 targetPort) {
		if (m_connected) {
			m_logger.LogWarning("Failed to connect to the SOCKS5 proxy. Error=Proxy connection already established!");

			return false;
		}

		SOCKET localSocket = ConnectToProxy();
		if (localSocket == INVALID_SOCKET) {
			return false;
		}

		if (!PerformHandshake(localSocket)) {
			return false;
		}

		if (!ConnectToTarget(localSocket, targetAddress, targetPort)) {
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

		shutdown(m_proxy, SD_BOTH);
		closesocket(m_proxy);

		m_proxy = INVALID_SOCKET;
		m_connected = false;

		message = std::format("Disconnected from the SOCKS5 proxy: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);
	}

	int Socks5Proxy::Send(const char* buffer, int length)
	{
		if (!m_connected || m_proxy == INVALID_SOCKET) {
			m_logger.LogError("Failed to send data through the SOCKS5 proxy. Error=Proxy is not connected");

			return false;
		}

		return SendExact(m_proxy, buffer, length);
	}

	int Socks5Proxy::Recv(char* buffer, int length)
	{
		if (!m_connected || m_proxy == INVALID_SOCKET) {
			m_logger.LogError("Failed to receive data from the SOCKS5 proxy. Error=Proxy is not connected");

			return false;
		}

		return recv(m_proxy, buffer, length, 0);
	}

	SOCKET Socks5Proxy::ConnectToProxy()
	{
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sock == INVALID_SOCKET) {
			m_logger.LogCritical("Failed to create SOCKS5 proxy socket.");

			return INVALID_SOCKET;
		}

		SOCKADDR_IN proxyAddr{};

		inet_pton(AF_INET, m_address.c_str(), &proxyAddr.sin_addr.s_addr);
		proxyAddr.sin_port = htons(m_port);
		proxyAddr.sin_family = AF_INET;

		std::string message = std::format("Connecting to the SOCKS5 proxy: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		if (connect(sock, reinterpret_cast<SOCKADDR*>(&proxyAddr), sizeof(proxyAddr)) == SOCKET_ERROR) {
			m_logger.LogCritical("Failed to connect to the SOCKS5 proxy.");
			closesocket(sock);

			return INVALID_SOCKET;
		}

		return sock;
	}

	bool Socks5Proxy::PerformHandshake(SOCKET sock)
	{
		bool requiresAuth = !m_username.empty() && !m_password.empty();
		
		char greeting[3] = { 0x05, 0x01, requiresAuth ? 0x02 : 0x00 };

		std::string message;

		message = std::format("Initiating the SOCKS5 proxy handshake: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		if (SendExact(sock, greeting, sizeof(greeting)) <= 0) {
			m_logger.LogCritical("Failed to initiate the SOCKS5 proxy handshake.");
			closesocket(sock);

			return false;
		}

		char response[2];
		if (RecvExact(sock, response, sizeof(response)) <= 0) {
			m_logger.LogCritical("Failed to complete the SOCKS5 proxy handshake.");
			closesocket(sock);

			return false;
		}

		if (!requiresAuth) {
			if (response[0] != 0x05 || response[1] != 0x00) {
				m_logger.LogCritical("Failed to negotiate 'No Auth' method with the SOCKS5 proxy.");
				closesocket(sock);

				return false;
			}
		}
		else {
			if (response[0] != 0x05 || response[1] != 0x02) {
				m_logger.LogCritical("Failed to negotiate 'Username/Password' method with the SOCKS5 proxy.");
				closesocket(sock);

				return false;
			}

			std::vector<char> authReq{
				0x01 // AUTH VERSION
			};

			authReq.push_back(static_cast<char>(m_username.length()));
			authReq.insert(authReq.end(), m_username.begin(), m_username.end());
			authReq.push_back(static_cast<char>(m_password.length()));
			authReq.insert(authReq.end(), m_password.begin(), m_password.end());

			if (SendExact(sock, authReq.data(), authReq.size()) <= 0) {
				m_logger.LogCritical("Failed to subnegotiate authentication.");
				closesocket(sock);

				return false;
			}

			char authResp[2];
			if (RecvExact(sock, authResp, sizeof(authResp)) <= 0) {
				m_logger.LogCritical("Failed to complete subnegotiation.");
				closesocket(sock);

				return false;
			}

			if (authResp[1] != 0x00) {
				m_logger.LogCritical("Rejected SOCKS5 Authentication. Error=Wrong username/password.");
				closesocket(sock);
			}
		}

		message = std::format("Finished the SOCKS5 proxy handshake: Endpoint={}:{}", m_address, m_port);
		m_logger.LogInfo(message);

		return true;
	}

	bool Socks5Proxy::ConnectToTarget(SOCKET sock, std::string_view targetAddress, UINT16 targetPort)
	{
		std::vector<char> connReq = {
			0x05, // VERSION 5
			0x01, // CMD: 0x01 (CONNECT)
			0x00, // Reserved: 0x00
			0x01, // Address Type: 0x01 (IPv4)
		};

		in_addr ipv4Addr{};
		if (inet_pton(AF_INET, targetAddress.data(), &ipv4Addr) != 1) {
			return false; // NOT IP
		}

		const char* ipBytes = reinterpret_cast<const char*>(&ipv4Addr.s_addr);

		connReq.insert(connReq.end(), ipBytes, ipBytes + 4);
		connReq.push_back((targetPort >> 8) & 0xFF);
		connReq.push_back(targetPort & 0xFF);

		if (SendExact(sock, connReq.data(), connReq.size()) <= 0) {
			m_logger.LogCritical("Failed to initiate the SOCKS5 proxy CONNECT request.");
			closesocket(sock);

			return false;
		}

		char connHeader[4];
		if (RecvExact(sock, connHeader, sizeof(connHeader)) <= 0) {
			m_logger.LogCritical("Failed to receive connection header from the SOCKS5 proxy.");
			closesocket(sock);

			return false;
		}

		std::string message;

		if (connHeader[1] != 0x00) {
			message = std::format("Rejected SOCKS5 connection to target. Status={}", static_cast<int>(connHeader[1]));
			m_logger.LogCritical(message);
			closesocket(sock);

			return false;
		}

		if (connHeader[3] == 0x01) { // IPv4
			char bindAddr[6];
			RecvExact(sock, bindAddr, sizeof(bindAddr));
		}
		else if (connHeader[3] == 0x04) { // IPv6
			char bindAddr[18];
			RecvExact(sock, bindAddr, sizeof(bindAddr));
		}
		else if (connHeader[3] == 0x03) { // Domain
			char len;
			RecvExact(sock, &len, sizeof(len));
			std::vector<char> domainAddr(len + 2);
			RecvExact(sock, domainAddr.data(), domainAddr.size());
		}

		message = std::format("Established connection to target via SOCKS5 proxy: Target={}:{}", targetAddress, targetPort);
		m_logger.LogInfo(message);

		return true;
	}

	int Socks5Proxy::SendExact(SOCKET sock, const char* buffer, int length)
	{
		int totalSent = 0;
		while (totalSent < length) {
			int bytes = send(sock, buffer + totalSent, length - totalSent, 0);
			if (bytes <= 0) {
				return -1;
			}

			totalSent += bytes;
		}

		return totalSent;
	}

	int Socks5Proxy::RecvExact(SOCKET sock, char* buffer, int length)
	{
		int totalReceived = 0;
		while (totalReceived < length) {
			int bytes = recv(sock, buffer + totalReceived, length - totalReceived, 0);
			if (bytes <= 0) {
				return -1;
			}

			totalReceived += bytes;
		}

		return totalReceived;
	}
}