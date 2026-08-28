#pragma once

#include <string>

#include "platform/sock_types.h"
#include "proxy/IProxy.h"
#include "io/IIoDriver.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class Socks5Proxy : public IProxy  {
	public:
		Socks5Proxy(std::string_view address, std::uint16_t port, IIoDriver& driver, ILogger& logger);
		Socks5Proxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IIoDriver& driver, ILogger& logger);
		~Socks5Proxy();

		bool Connect(std::string_view targetAddress, std::uint16_t targetPort) override;
		void Disconnect() override;

		void Send(std::span<const std::byte> buffer, std::function<void(const IoResult&)> callback) override;
		void Recv(std::span<std::byte> buffer, std::function<void(const IoResult&)> callback) override;
	private:
		NativeSocket m_proxy{ InvalidNativeSocket };

		std::string m_address;
		std::uint16_t m_port;
		std::string m_username;
		std::string m_password;

		IIoDriver& m_driver;
		ILogger& m_logger;

		bool m_connected{ false };
	
	protected:
		virtual NativeSocket ConnectToProxy();
		virtual bool PerformHandshake(NativeSocket sock);
		virtual bool ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort);

		static bool SendExact(NativeSocket sock, std::span<const char> buffer);
		static bool RecvExact(NativeSocket sock, std::span<char> buffer);
	};
}