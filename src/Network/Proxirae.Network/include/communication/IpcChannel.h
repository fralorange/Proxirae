#pragma once

#include <unordered_map>
#include <mutex>

#include "communication/ISendChannel.h"
#include "communication/IReceiveChannel.h"
#include "communication/IDispatchChannel.h"
#include "communication/IPipeServer.h"

namespace Proxirae {
	class IpcChannel : public ISendChannel, public IReceiveChannel, public IDispatchChannel {
	public:
		explicit IpcChannel(IPipeServer& pipe);

		bool Accept() override;

		bool Send(const PipeMessage& msg) override;
		void Receive(MessageCallback onMessage, ErrorCallback onError) override;

		void Dispatch(const PipeMessage& msg) override;
		void RegisterHandler(PipeMessageType messageType, IPipeMessageHandler& handler) override;

	private:
		IPipeServer& m_pipe;
		std::unordered_map<PipeMessageType, IPipeMessageHandler*> m_handlers;
		std::mutex m_handlersMutex;

		std::vector<std::byte> m_readBuffer = std::vector<std::byte>(1024 * 4);
		MessageCallback m_onMessage;
		ErrorCallback m_onError;

		void StartReceive();
	};
}