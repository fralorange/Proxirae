#include <memory>
#include <future>

#include "ipc/IpcChannel.h"

namespace Proxirae {
	IpcChannel::IpcChannel(IPipeServer& pipe)
		: m_pipe(pipe) { }

	bool IpcChannel::Accept()
	{
		return m_pipe.Accept();
	}

	bool IpcChannel::Send(const PipeMessage& msg)
	{
		PipeMessageHeader header = msg.header;
		header.magic = 0x5850;
		header.payloadSize = static_cast<uint32_t>(msg.payload.size());

		auto packet = std::make_shared<std::vector<std::byte>>(sizeof(PipeMessageHeader) + msg.payload.size());

		std::memcpy(packet->data(), &header, sizeof(PipeMessageHeader));
		if (!msg.payload.empty()) {
			std::memcpy(packet->data() + sizeof(PipeMessageHeader), msg.payload.data(), msg.payload.size());
		}

		m_pipe.AsyncWrite(*packet, [this, packet](IoResult result) {
			if (!result.success && m_onError) {
				/*m_onError();*/
			}
		});

		return true;
	}

	void IpcChannel::Receive(MessageCallback onMessage, ErrorCallback onError)
	{
		m_onMessage = std::move(onMessage);
		m_onError = std::move(onError);

		StartReceive();
	}

	void IpcChannel::Dispatch(const PipeMessage& msg)
	{
		IPipeMessageHandler* handler{};
		{
			std::lock_guard<std::mutex> lock(m_handlersMutex);
			auto it = m_handlers.find(msg.header.type);
			if (it != m_handlers.end()) handler = it->second;
		}

		if (handler) {
			handler->Handle(msg);
		}
	}

	void IpcChannel::RegisterHandler(PipeMessageType messageType, IPipeMessageHandler& handler)
	{
		std::lock_guard<std::mutex> lock(m_handlersMutex);
		m_handlers[messageType] = &handler;
	}

	void IpcChannel::StartReceive() {
		m_pipe.AsyncRead(m_readBuffer, [this](IoResult result) {
			if (!result.success || result.bytesTransferred == 0) {
				if (m_onError) m_onError();
				return;
			}

			std::span<const std::byte> validData{ m_readBuffer.data(), result.bytesTransferred };
			auto msg = PipeMessage::TryParse(validData);

			if (msg && m_onMessage) {
				m_onMessage(*msg);
			}

			StartReceive();
		});
	}
}