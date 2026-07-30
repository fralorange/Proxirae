#include <format>

#include "core/PacketDiverter.h"

namespace Proxirae {
	PacketDiverter::PacketDiverter(ILogger& logger) 
		: m_logger(logger) {}

	PacketDiverter::~PacketDiverter()
	{
		Close();
	}

	bool PacketDiverter::Open()
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

	void PacketDiverter::Close()
	{
		if (m_handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		WinDivertShutdown(m_handle, WINDIVERT_SHUTDOWN_BOTH);
		WinDivertClose(m_handle);

		m_handle = INVALID_HANDLE_VALUE;
	}

	std::optional<Packet> PacketDiverter::Receive()
	{
		if (!WinDivertRecv(m_handle, m_packetBuffer.data(), m_packetBuffer.size(), &m_packetLen, &m_packetAddr)) {
			std::string message = std::format("Failed to receive packet. Error={}", GetLastError());
			m_logger.LogError(message);

			return std::nullopt;
		}

		auto packetOpt = Packet::TryCreate(m_packetBuffer.data(), m_packetLen, m_packetAddr);

		if (!packetOpt.has_value()) {
			m_logger.LogError("Failed to parse and create packet.");

			return std::nullopt;
		}

		return packetOpt;
	}

	bool PacketDiverter::Send(Packet& packet)
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