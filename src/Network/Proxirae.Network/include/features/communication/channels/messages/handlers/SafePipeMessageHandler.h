#pragma once

#include "IPipeMessageHandler.h"
#include "features/diagnostics/ILogger.h"

namespace Proxirae {
    class SafePipeMessageHandler : public IPipeMessageHandler {
    public:
        explicit SafePipeMessageHandler(ILogger& logger);
        ~SafePipeMessageHandler() override = default;

        void Handle(const PipeMessage& msg) final override;

    protected:
        virtual void HandleUnsafe(const PipeMessage& msg) = 0;

        ILogger& m_logger;
    };
}