#pragma once

#include <stop_token>

#include "packet/IPacketDiverter.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketRouter.h"

namespace Proxirae {
    class Engine {
    public:
        Engine(IPacketDiverter& diverter, PacketDispatcher& dispatcher, PacketRouter& router, std::stop_token token);
        ~Engine();

        void Run();

    private:
        IPacketDiverter& m_diverter;
        PacketDispatcher& m_dispatcher;
        PacketRouter& m_router;
        std::stop_token m_token;
    };
}