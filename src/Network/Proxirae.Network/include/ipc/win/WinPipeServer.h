#pragma once

#include <Windows.h>

#include "ipc/IPipeServer.h"
#include "io/IIoDriver.h"

namespace Proxirae {
	class WinPipeServer : public IPipeServer {
	public:
		WinPipeServer(IIoDriver& driver);
		~WinPipeServer();

		bool Accept() override;

		void AsyncRead(std::span<std::byte> buffer, IoCallback callback) override;
		void AsyncWrite(std::span<const std::byte> buffer, IoCallback callback) override;

	private:
		HANDLE m_pipe = INVALID_HANDLE_VALUE;
		IIoDriver& m_driver;
	};
}