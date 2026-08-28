#include "application/Engine.h"

namespace Proxirae {
	Engine::Engine(IPacketDiverter& diverter, PacketDispatcher& dispatcher, PacketRouter& router, std::stop_token token)
		: m_diverter(diverter), m_dispatcher(dispatcher), m_router(router), m_token(token) {}

	Engine::~Engine() {
		m_diverter.Close();
	}

	void Engine::Run() {
		if (!m_diverter.Open()) {
			return;
		};

		std::stop_callback interruptCallback(m_token, [this]() {
			m_diverter.Interrupt();
		});

		while (!m_token.stop_requested()) {
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
