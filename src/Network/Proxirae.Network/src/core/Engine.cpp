#include "core/Engine.h"

namespace Proxirae {
	Engine::Engine(IPacketDiverter& diverter, PacketDispatcher& dispatcher, PacketRouter& router)
		: m_diverter(diverter), m_dispatcher(dispatcher), m_router(router) {}

	Engine::~Engine() {
		m_diverter.Close();
	}

	void Engine::Run() {
		if (!m_diverter.Open()) {
			return;
		}

		m_running = true;

		while (m_running) {
			m_diverter.Receive([this](IPacketContext& ctx) {
				auto route = m_router.Route(ctx);
				
				switch (route.action) {
					case RuleAction::Direct:
						break;
					case RuleAction::Block:
						return;
					case RuleAction::Proxy:
						m_dispatcher.Dispatch({ctx, route.proxyId});
						break;
				}

				m_diverter.Send(ctx);
			});
		}
	}
}
