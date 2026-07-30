#include <string>
#include <format>
#include <ws2tcpip.h>

#include "core/Engine.h"
#include "core/Packet.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr uint16_t DAEMON_PORT = 33999;

namespace Proxirae {
	Engine::Engine(PacketDiverter& diverter, PacketDispatcher& dispatcher)
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
			auto packetOpt = m_diverter.Receive();

			if (!packetOpt.has_value()) {
				continue;
			}

			auto& packet = packetOpt.value();

			m_dispatcher.Dispatch(packet);

			m_diverter.Send(packet);
		}
	}
}
