#include "ipc/win/WinPipeServer.h"

namespace Proxirae {
	WinPipeServer::WinPipeServer(IIoDriver& driver)
		: m_driver(driver)
	{
		m_pipe = CreateNamedPipeW(
			LR"(\\.\pipe\Proxirae.Network)",
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		 	PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1,
			1024 * 4,
			1024 * 4,
			0,
			nullptr
		);
	}

	WinPipeServer::~WinPipeServer()
	{
		if (m_pipe != INVALID_HANDLE_VALUE) {
			CloseHandle(m_pipe);
			m_pipe = INVALID_HANDLE_VALUE;
		}
	}

	bool WinPipeServer::Accept()
	{
		if (m_pipe == INVALID_HANDLE_VALUE) {
			return false;
		}

		OVERLAPPED overlapped{};
		overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
		if (!overlapped.hEvent) return false;

		bool connected = false;
		if (ConnectNamedPipe(m_pipe, &overlapped)) {
			connected = true;
		}
		else {
			DWORD err = GetLastError();
			if (err == ERROR_PIPE_CONNECTED) {
				connected = true;
			}
			else if (err == ERROR_IO_PENDING) {
				DWORD bytes = 0;
				connected = GetOverlappedResult(m_pipe, &overlapped, &bytes, TRUE);
			}
		}

		CloseHandle(overlapped.hEvent);

		if (m_pipe != INVALID_HANDLE_VALUE) {
			m_driver.Attach(reinterpret_cast<NativeHandle>(m_pipe));
		}

		return connected;
	}

	void WinPipeServer::AsyncRead(std::span<std::byte> buffer, IoCallback callback)
	{
		if (m_pipe == INVALID_HANDLE_VALUE) {
			callback(IoResult{ false, 0, static_cast<int>(ERROR_INVALID_HANDLE) });
			return;
		}

		m_driver.AsyncRead(reinterpret_cast<NativeHandle>(m_pipe), buffer, std::move(callback));
	}

	void WinPipeServer::AsyncWrite(std::span<const std::byte> buffer, IoCallback callback)
	{
		if (m_pipe == INVALID_HANDLE_VALUE) {
			callback(IoResult{ false, 0, static_cast<int>(ERROR_INVALID_HANDLE) });
			return;
		}

		m_driver.AsyncWrite(reinterpret_cast<NativeHandle>(m_pipe), buffer, std::move(callback));
	}
}