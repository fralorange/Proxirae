#include <thread>
#include <semaphore>

#include "runtime/Host.h"

namespace Proxirae {
	Host::Host(IReceiveChannel& receiver, IDispatchChannel& dispatcher, std::stop_source source)
		: m_receiver(receiver), m_dispatcher(dispatcher), m_source(source) { }

	void Host::Run() {
		bool connected = false;
		while (!m_source.stop_requested()) {
			if (m_receiver.Accept()) {
				connected = true;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (!connected || m_source.stop_requested()) {
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

		while (!sessionsDone->try_acquire_for(std::chrono::milliseconds(50))) {
			if (m_source.stop_requested()) {
				break;
			}
		}
	}
}