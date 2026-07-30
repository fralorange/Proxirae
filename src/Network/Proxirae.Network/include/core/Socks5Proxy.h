#pragma once

#include <string>

#include "core/IProxy.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class Socks5Proxy : public IProxy  {
	public:
		Socks5Proxy(std::string_view address, UINT16 port, ILogger& logger);
		Socks5Proxy(std::string_view address, UINT16 port, std::string_view username, std::string_view password, ILogger& logger);
		~Socks5Proxy();

		bool Connect(std::string_view targetAddress, UINT16 targetPort) override;
		void Disconnect() override;

		int Send(const char* buffer, int length) override;
		int Recv(char* buffer, int length) override;
	private:
		SOCKET m_proxy{ INVALID_SOCKET };

		std::string m_address;
		UINT16 m_port;
		std::string m_username;
		std::string m_password;

		ILogger& m_logger;

		bool m_connected{ false };

		SOCKET ConnectToProxy();
		bool PerformHandshake(SOCKET sock);
		bool ConnectToTarget(SOCKET sock, std::string_view targetAddress, UINT16 targetPort);

		static int SendExact(SOCKET sock, const char* buffer, int length);
		static int RecvExact(SOCKET sock, char* buffer, int length);
	};
}