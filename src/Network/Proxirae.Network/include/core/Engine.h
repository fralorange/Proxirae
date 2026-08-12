#pragma once

#include "core/packet/IPacketDiverter.h"
#include "core/packet/PacketDispatcher.h"
#include "core/packet/PacketRouter.h"

namespace Proxirae {
    class Engine {
    public:
        Engine(IPacketDiverter& diverter, PacketDispatcher& dispatcher, PacketRouter& router);
        ~Engine();

        void Run();

    private:
        IPacketDiverter& m_diverter;
        PacketDispatcher& m_dispatcher;
        PacketRouter& m_router;

        bool m_running{ false };
    };
}