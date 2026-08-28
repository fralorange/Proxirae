#include <format>
#include <windivert.h>
#include <vector>

#include "interception/win/WinPacketContext.h"
#include "interception/win/PacketWinDiverter.h"

namespace Proxirae {
	class PacketWinDiverter::DivertChannel {
	public:
		HANDLE handle{ INVALID_HANDLE_VALUE };
		HANDLE event{ nullptr };

		OVERLAPPED overlapped{};

		WINDIVERT_ADDRESS addr{};
		std::uint32_t addrSize{ sizeof(WINDIVERT_ADDRESS) };

		std::vector<std::uint8_t> buffer;
		std::uint32_t packetLen{ 0 };

		DivertChannel(size_t bufferSize = 0) {
			if (bufferSize > 0) {
				buffer.resize(bufferSize);
			}
		}

		~DivertChannel() {
			Close();
		}

		bool Open(const char* filter, WINDIVERT_LAYER layer, INT16 priority, UINT64 flags) {
			handle = WinDivertOpen(filter, layer, priority, flags);

			if (handle == INVALID_HANDLE_VALUE) {
				return false;
			}

			event = CreateEvent(nullptr, TRUE, FALSE, nullptr);

			return true;
		}

		void Close() {
			if (handle != INVALID_HANDLE_VALUE) {
				WinDivertShutdown(handle, WINDIVERT_SHUTDOWN_BOTH);
				WinDivertClose(handle);
				handle = INVALID_HANDLE_VALUE;
			}
			if (event) {
				CloseHandle(event); 
				event = nullptr;
			}
		}

		void Interrupt() {
			if (handle != INVALID_HANDLE_VALUE) {
				WinDivertShutdown(handle, WINDIVERT_SHUTDOWN_BOTH);
			}
		}

		bool StartReceive() {
			ResetEvent(event);

			std::memset(&overlapped, 0, sizeof(overlapped));

			overlapped.hEvent = event;

			addrSize = sizeof(WINDIVERT_ADDRESS);
			packetLen = 0;

			BOOL result = WinDivertRecvEx(
				handle,
				buffer.data(),
				static_cast<std::uint32_t>(buffer.size()),
				&packetLen,
				0,
				&addr,
				&addrSize,
				&overlapped
			);

			if (result || GetLastError() == ERROR_IO_PENDING) {
				return true;
			}

			return false;
		}
	};

	PacketWinDiverter::PacketWinDiverter(WinTcpCorrelator& tcpCorrelator, ILogger& logger)
		: m_tcpCorrelator(tcpCorrelator), 
		  m_logger(logger), 
		  m_network(std::make_unique<DivertChannel>(65535)),
		  m_socket(std::make_unique<DivertChannel>(0)) { }

	PacketWinDiverter::~PacketWinDiverter()
	{
		Close();
	}

	bool PacketWinDiverter::Open()
	{
		const char* networkFilter = "ip and tcp and tcp.SrcPort != 25344 and tcp.DstPort != 25344 and tcp.SrcPort != 10808 and tcp.DstPort != 10808";
		const char* socketFilter = "tcp and (event == CONNECT or event == CLOSE)";

		if (!m_network->Open(networkFilter, WINDIVERT_LAYER_NETWORK, 10, 0))
		{
			return false;
		}

		if (!m_socket->Open(socketFilter, WINDIVERT_LAYER_SOCKET, 100, WINDIVERT_FLAG_RECV_ONLY | WINDIVERT_FLAG_SNIFF))
		{
			m_network->Close();

			return false;
		}

		if (!m_network->StartReceive()) {
			Close();

			return false;
		}

		if (!m_socket->StartReceive()) {
			Close();
			
			return false;
		}

		return true;
	}

	void PacketWinDiverter::Close()
	{
		m_network->Close();
		m_socket->Close();
	}

	void PacketWinDiverter::Interrupt()
	{
		m_network->Interrupt();
		m_socket->Interrupt();
	}

	bool PacketWinDiverter::Receive(const std::function<void(IPacketContext&)>& callback)
	{
		HANDLE events[2] = { m_network->event, m_socket->event };

		std::uint32_t result = static_cast<std::uint32_t>(WaitForMultipleObjects(2, events, FALSE, INFINITE));

		if (result == WAIT_OBJECT_0) {
			DWORD bytesTransferred = 0;

			if (!GetOverlappedResult(m_network->handle, &m_network->overlapped, &bytesTransferred, FALSE)) {
				DWORD error = GetLastError();

				if (error == ERROR_NO_DATA) {
					return false;
				}

				m_logger.LogError(std::format("[WinDivert] Network receive overlapped result failed: error {}", error));
				return false;
			}

			Packet packet{ std::vector<std::uint8_t>(m_network->buffer.begin(), m_network->buffer.begin() + bytesTransferred), bytesTransferred, m_network->addr };
			m_tcpCorrelator.CorrelateNetwork(packet, callback);

			m_network->StartReceive();

			return true;
		}

		if (result == WAIT_OBJECT_0 + 1) {
			DWORD bytesTransferred = 0;

			if (!GetOverlappedResult(m_socket->handle, &m_socket->overlapped, &bytesTransferred, FALSE)) {
				DWORD error = GetLastError();

				if (error == ERROR_NO_DATA) {
					return false;
				}

				m_logger.LogError(std::format("[WinDivert] Socket receive overlapped result failed: error {}", error));

				return false;
			}

			m_tcpCorrelator.CorrelateSocket(m_socket->addr, callback);
			
			m_socket->StartReceive();

			return true;
		}

		return false;
	}

	bool PacketWinDiverter::Send(IPacketContext& packet)
	{
		if (packet.IsModified() && !WinDivertHelperCalcChecksums(packet.GetRawData(), packet.GetRawDataLength(), &packet.GetMetadata(), 0)) {
			m_logger.LogError("[WinDivert] Failed to calculate packet checksums");

			return false;
		}

		if (!WinDivertSend(m_network->handle, packet.GetRawData(), packet.GetRawDataLength(), nullptr, &packet.GetMetadata())) {
			m_logger.LogError(std::format("[WinDivert] Failed to send packet: error {}", GetLastError()));

			return false;
		}

		return true;
	}
}