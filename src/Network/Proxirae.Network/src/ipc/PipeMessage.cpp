#include "ipc/PipeMessage.h"

namespace Proxirae {
	PipeMessage PipeMessage::Create(PipeMessageType type)
	{
		return PipeMessage{
			.header = PipeMessageHeader{
				.type = type
			}
		};
	}

	PipeMessage PipeMessage::Create(PipeMessageType type, std::vector<std::byte> payload)
	{
		return PipeMessage{
			.header = PipeMessageHeader{
				.type = type
			},
			.payload = payload
		};
	}

	std::optional<PipeMessage> Proxirae::PipeMessage::TryParse(std::span<const std::byte> rawData)
	{
		if (rawData.size() < sizeof(PipeMessageHeader)) {
			return std::nullopt;
		}

		PipeMessage msg;
		std::memcpy(&msg.header, rawData.data(), sizeof(PipeMessageHeader));

		if (msg.header.magic != 0x5850) {
			return std::nullopt;
		}

		if (msg.header.payloadSize > 0) {
			if (rawData.size() < sizeof(PipeMessageHeader) + msg.header.payloadSize) {
				return std::nullopt;
			}

			auto payloadStart = rawData.begin() + sizeof(PipeMessageHeader);
			msg.payload.assign(payloadStart, payloadStart + msg.header.payloadSize);
		}

		return msg;
	}
}