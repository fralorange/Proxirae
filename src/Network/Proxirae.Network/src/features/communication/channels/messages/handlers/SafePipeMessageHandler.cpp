#include <format>

#include "features/communication/channels/messages/handlers/SafePipeMessageHandler.h"

namespace Proxirae {
	SafePipeMessageHandler::SafePipeMessageHandler(ILogger& logger)
		: m_logger(logger) { }

    void SafePipeMessageHandler::Handle(const PipeMessage& msg)
    {
        try {
            HandleUnsafe(msg);
        }
        catch (const std::bad_alloc&) {
            throw;
        }
        catch (const std::exception& e) {
            m_logger.LogError(std::format(
                "[PipeMessageHandler] Message handler failed: {}", e.what()
            ));
        }
        catch (...) {
            m_logger.LogCritical(
                "[PipeMessageHandler] Message handler failed with an unknown exception"
            );
        }
    }
}
