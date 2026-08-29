#pragma once
#include <functional>
#include <stop_token>

#include "communication/PipeMessage.h"

namespace Proxirae {
	using MessageCallback = std::function<void(const PipeMessage&)>;
    using ErrorCallback = std::function<void()>;

	class IReceiveChannel {
	public:
		virtual ~IReceiveChannel() = default;

		virtual bool Accept(std::stop_token token) = 0;

		virtual void Receive(MessageCallback onMessage, ErrorCallback onError) = 0;
	};
}