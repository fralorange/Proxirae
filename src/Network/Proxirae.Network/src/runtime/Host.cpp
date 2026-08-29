#include <thread>
#include <semaphore>

#include "runtime/Host.h"

namespace Proxirae {
	Host::Host(IReceiveChannel& receiver, IDispatchChannel& dispatcher, std::stop_source source)
		: m_receiver(receiver), m_dispatcher(dispatcher), m_source(source) { }

	void Host::Run() {
		if (!m_receiver.Accept(m_source.get_token())) {
			return;
		}

		auto sessionsDone = std::make_shared<std::binary_semaphore>(0);

		m_receiver.Receive(
			[this](const PipeMessage& msg) {
				m_dispatcher.Dispatch(msg);
			},
			[this, sessionsDone]() {
				m_source.request_stop();
				sessionsDone->release();
			}
		);

		std::stop_callback stopCallback(m_source.get_token(), [sessionsDone]() {
			sessionsDone->release();
		});

		sessionsDone->acquire();
	}
}