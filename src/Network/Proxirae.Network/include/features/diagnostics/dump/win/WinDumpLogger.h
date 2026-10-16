#pragma once

#include <fstream>
#include <filesystem>
#include <mutex>

#include "features/diagnostics/ILogger.h"

namespace Proxirae {
    class WinDumpLogger final : public ILogger {
    public:
        WinDumpLogger();

        void Log(LogLevel level, std::string_view message) override;

    private:
        std::filesystem::path m_logPath;
        std::ofstream m_file;
        std::mutex m_mutex;
    };
}