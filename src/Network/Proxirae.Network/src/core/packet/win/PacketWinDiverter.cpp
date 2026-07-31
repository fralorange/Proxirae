#include <format>

#include "core/packet/win/WinPacketContext.h"
#include "core/packet/win/PacketWinDiverter.h"

namespace Proxirae {
	PacketWinDiverter::PacketWinDiverter(ILogger& logger) 
		: m_logger(logger) {}

	PacketWinDiverter::~PacketWinDiverter()
	{
		Close();
	}

	bool PacketWinDiverter::Open()
	{
		const char* filter = "ip and tcp and tcp.SrcPort != 25344 and tcp.DstPort != 25344 and tcp.SrcPort != 10808 and tcp.DstPort != 10808";

		m_handle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 123, 0);

		if (m_handle == INVALID_HANDLE_VALUE)
		{
			std::string message = std::format("Failed to open WinDivert handle. Error={}", GetLastError());
			m_logger.LogCritical(message);

			return false;
		}

		return true;
	}

	void PacketWinDiverter::Close()
	{
		if (m_handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		WinDivertShutdown(m_handle, WINDIVERT_SHUTDOWN_BOTH);
		WinDivertClose(m_handle);

		m_handle = INVALID_HANDLE_VALUE;
	}

	bool PacketWinDiverter::Receive(const std::function<void(IPacketContext&)>& ctx)
	{
		if (!WinDivertRecv(m_handle, m_packetBuffer.data(), static_cast<std::uint32_t>(m_packetBuffer.size()), &m_packetLen, &m_packetAddr)) {
			std::string message = std::format("Failed to receive packet. Error={}", GetLastError());
			m_logger.LogError(message);

			return false;
		}

		auto packetOpt = WinPacketContext::TryCreate(m_packetBuffer.data(), m_packetLen, m_packetAddr);

		if (!packetOpt.has_value()) {
			m_logger.LogError("Failed to parse and create packet.");

			return false;
		}

		ctx(packetOpt.value());

		return true;
	}

	bool PacketWinDiverter::Send(IPacketContext& packet)
	{
		if (packet.IsModified() && !WinDivertHelperCalcChecksums(m_packetBuffer.data(), m_packetLen, &m_packetAddr, 0)) {
			m_logger.LogError("Failed to calculate checksums.");

			return false;
		}

		if (!WinDivertSend(m_handle, m_packetBuffer.data(), m_packetLen, nullptr, &m_packetAddr)) {
			std::string message = std::format("Failed to send packet. Error={}", GetLastError());
			m_logger.LogError(message);

			return false;
		}

		return true;
	}
}