#pragma once
#include "PipeMessage.h"

namespace Proxirae {
	class IPipeMessageHandler {
	public:
		virtual ~IPipeMessageHandler() = default;

		virtual void Handle(const PipeMessage& msg) = 0;
	};
}