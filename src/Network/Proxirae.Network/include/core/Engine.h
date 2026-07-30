#pragma once

#include <windivert.h>

#include "core/PacketDiverter.h"
#include "core/PacketDispatcher.h"

namespace Proxirae {
    class Engine {
    public:
        Engine(PacketDiverter& diverter, PacketDispatcher& dispatcher);
        ~Engine();

        void Run();

    private:
        PacketDiverter& m_diverter;
        PacketDispatcher& m_dispatcher;

        bool m_running{ false };
    };
}