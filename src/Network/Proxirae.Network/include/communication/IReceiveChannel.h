#pragma once
#include <functional>

#include "communication/PipeMessage.h"

namespace Proxirae {
	using MessageCallback = std::function<void(const PipeMessage&)>;
    using ErrorCallback = std::function<void()>;

	class IReceiveChannel {
	public:
		virtual ~IReceiveChannel() = default;

		virtual bool Accept() = 0;

		virtual void Receive(MessageCallback onMessage, ErrorCallback onError) = 0;
	};
}