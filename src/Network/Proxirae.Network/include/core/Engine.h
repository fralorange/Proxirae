#pragma once

#include "core/packet/IPacketDiverter.h"
#include "core/packet/PacketDispatcher.h"

namespace Proxirae {
    class Engine {
    public:
        Engine(IPacketDiverter& diverter, PacketDispatcher& dispatcher);
        ~Engine();

        void Run();

    private:
        IPacketDiverter& m_diverter;
        PacketDispatcher& m_dispatcher;

        bool m_running{ false };
    };
}