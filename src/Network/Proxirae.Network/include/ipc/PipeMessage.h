#pragma once

#include <vector>
#include <optional>
#include <span>

#include "ipc/PipeMessageHeader.h"

namespace Proxirae {
	struct PipeMessage {
		PipeMessageHeader header;
		std::vector<std::byte> payload; // JSON

		static PipeMessage Create(PipeMessageType type);
		static PipeMessage Create(PipeMessageType type, std::vector<std::byte> payload);
		static std::optional<PipeMessage> TryParse(std::span<const std::byte> rawData);
	};
}