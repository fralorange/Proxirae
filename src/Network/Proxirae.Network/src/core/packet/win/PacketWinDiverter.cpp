#include <format>

#include "core/packet/win/WinPacketContext.h"
#include "core/packet/win/PacketWinDiverter.h"

namespace Proxirae {
	PacketWinDiverter::PacketWinDiverter(WinTcpCorrelator& tcpCorrelator, ILogger& logger)
		: m_tcpCorrelator(tcpCorrelator), m_logger(logger) { }

	PacketWinDiverter::~PacketWinDiverter()
	{
		Close();
	}

	bool PacketWinDiverter::Open()
	{
		const char* networkFilter = "ip and tcp and tcp.SrcPort != 25344 and tcp.DstPort != 25344 and tcp.SrcPort != 10808 and tcp.DstPort != 10808";
		const char* socketFilter = "tcp and (event == CONNECT or event == CLOSE)";

		if (!OpenNetwork(networkFilter))
		{
			return false;
		}

		if (!OpenSocket(socketFilter))
		{
			CloseHandle(m_networkHandle);

			return false;
		}

		if (!NetworkReceive()) {
			Close();

			return false;
		}

		if (!SocketReceive()) {
			Close();
			
			return false;
		}

		return true;
	}

	void PacketWinDiverter::Close()
	{
		CloseHandle(m_networkHandle);
		CloseHandle(m_socketHandle);
	}

	bool PacketWinDiverter::Receive(const std::function<void(IPacketContext&)>& callback)
	{
		HANDLE events[2] = { m_networkEvent, m_socketEvent };

		std::uint32_t result = static_cast<std::uint32_t>(WaitForMultipleObjects(2, events, FALSE, INFINITE));

		if (result == WAIT_OBJECT_0) {
			DWORD bytesTransferred = 0;

			if (!GetOverlappedResult(m_networkHandle, &m_networkOverlapped, &bytesTransferred, FALSE)) {
				std::string message = std::format("Failed to get overlapped result for network receive. Error={}", GetLastError());
				m_logger.LogError(message);
				return false;
			}

			Packet packet{ std::vector<std::uint8_t>(m_networkPacketBuffer.begin(), m_networkPacketBuffer.begin() + bytesTransferred), bytesTransferred, m_networkPacketAddr };
			m_tcpCorrelator.CorrelateNetwork(packet, callback);

			NetworkReceive();

			return true;
		}

		if (result == WAIT_OBJECT_0 + 1) {
			DWORD bytesTransferred = 0;

			if (!GetOverlappedResult(m_socketHandle, &m_socketOverlapped, &bytesTransferred, FALSE)) {
				std::string message = std::format("Failed to get overlapped result for socket receive. Error={}", GetLastError());
				m_logger.LogError(message);

				return false;
			}

			m_tcpCorrelator.CorrelateSocket(m_socketPacketAddr, callback);
			
			SocketReceive();

			return true;
		}

		return false;
	}

	bool PacketWinDiverter::Send(IPacketContext& packet)
	{
		if (packet.IsModified() && !WinDivertHelperCalcChecksums(packet.GetRawData(), packet.GetRawDataLength(), &packet.GetMetadata(), 0)) {
			m_logger.LogError("Failed to calculate checksums.");

			return false;
		}

		if (!WinDivertSend(m_networkHandle, packet.GetRawData(), packet.GetRawDataLength(), nullptr, &packet.GetMetadata())) {
			std::string message = std::format("Failed to send packet. Error={}", GetLastError());
			m_logger.LogError(message);

			return false;
		}

		return true;
	}

	bool PacketWinDiverter::OpenNetwork(const char* filter)
	{
		m_networkHandle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 10, 0);

		if (m_networkHandle == INVALID_HANDLE_VALUE)
		{
			std::string message = std::format("Failed to open WinDivert network handle. Error={}", GetLastError());
			m_logger.LogCritical(message);

			return false;
		}

		m_networkEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		return true;
	}

	bool PacketWinDiverter::OpenSocket(const char* filter)
	{
		m_socketHandle = WinDivertOpen(filter, WINDIVERT_LAYER_SOCKET, 100, WINDIVERT_FLAG_RECV_ONLY | WINDIVERT_FLAG_SNIFF);

		if (m_socketHandle == INVALID_HANDLE_VALUE)
		{
			std::string message = std::format("Failed to open WinDivert socket handle. Error={}", GetLastError());
			m_logger.LogCritical(message);
			return false;
		}

		m_socketEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		return true;
	}

	void PacketWinDiverter::CloseHandle(HANDLE& handle)
	{
		if (handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		WinDivertShutdown(handle, WINDIVERT_SHUTDOWN_BOTH);
		WinDivertClose(handle);

		handle = INVALID_HANDLE_VALUE;
	}

	bool PacketWinDiverter::NetworkReceive()
	{
		ResetEvent(m_networkEvent);

		std::memset(&m_networkOverlapped, 0, sizeof(m_networkOverlapped));

		m_networkOverlapped.hEvent = m_networkEvent;

		m_networkPacketAddrSize = sizeof(WINDIVERT_ADDRESS);
		m_networkPacketLen = 0;

		BOOL result = WinDivertRecvEx(
			m_networkHandle,
			m_networkPacketBuffer.data(),
			static_cast<std::uint32_t>(m_networkPacketBuffer.size()),
			&m_networkPacketLen,
			0,
			&m_networkPacketAddr,
			&m_networkPacketAddrSize,
			&m_networkOverlapped);

		if (result) {
			return true;
		}

		std::uint32_t error = static_cast<std::uint32_t>(GetLastError());

		if (error == ERROR_IO_PENDING) {
			return true;
		}

		m_logger.LogError(std::format("Failed to start asynchronous network receive. Error={}", error));

		return false;
	}

	bool PacketWinDiverter::SocketReceive()
	{
		ResetEvent(m_socketEvent);

		std::memset(&m_socketOverlapped, 0, sizeof(m_socketOverlapped));

		m_socketOverlapped.hEvent = m_socketEvent;

		m_socketPacketAddrSize = sizeof(WINDIVERT_ADDRESS);

		BOOL result = WinDivertRecvEx(
			m_socketHandle,
			nullptr,
			0,
			nullptr,
			0,
			&m_socketPacketAddr,
			&m_socketPacketAddrSize,
			&m_socketOverlapped);

		if (result) {
			return true;
		}

		std::uint32_t error = static_cast<std::uint32_t>(GetLastError());

		if (error == ERROR_IO_PENDING) {
			return true;
		}

		m_logger.LogError(std::format("Failed to start asynchronous socket receive. Error={}", error));

		return false;
	}
}