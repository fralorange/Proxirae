#include "core/Engine.h"

namespace Proxirae {
	Engine::Engine(IPacketDiverter& diverter, PacketDispatcher& dispatcher)
		: m_diverter(diverter), m_dispatcher(dispatcher) {}

	Engine::~Engine() {
		m_diverter.Close();
	}

	void Engine::Run() {
		if (!m_diverter.Open()) {
			return;
		}

		m_running = true;

		while (m_running) {
			m_diverter.Receive([this](IPacketContext& packet) {
				m_dispatcher.Dispatch(packet);
				m_diverter.Send(packet);
			});
		}
	}
}
