#pragma once

#include <Windows.h>

#include "communication/IPipeServer.h"
#include "asyncio/async/IAsyncDriver.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"

namespace Proxirae {
	class WinPipeServer : public IPipeServer {
	public:
		WinPipeServer(IAsyncDriver& driver, IIoStreamAdapter& adapter);
		~WinPipeServer();

		bool Accept(std::stop_token token) override;

		void AsyncRead(std::span<std::byte> buffer, IoCallback callback) override;
		void AsyncWrite(std::span<const std::byte> buffer, IoCallback callback) override;

	private:
		HANDLE m_pipe = INVALID_HANDLE_VALUE;

		IAsyncDriver& m_driver;
		IIoStreamAdapter& m_adapter;
	};
}