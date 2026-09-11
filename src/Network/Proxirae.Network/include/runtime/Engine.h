#pragma once

#include <stop_token>

#include "interception/diversion/IPacketDiverter.h"
#include "interception/handling/IPacketHandler.h"
#include "interception/routing/PacketRouter.h"

namespace Proxirae {
    class Engine {
    public:
        Engine(IPacketDiverter& diverter, IPacketHandler& handler, PacketRouter& router, std::stop_token token);
        ~Engine();

        void Run();

    private:
        IPacketDiverter& m_diverter;
        IPacketHandler& m_handler;
        PacketRouter& m_router;
        std::stop_token m_token;
    };
}