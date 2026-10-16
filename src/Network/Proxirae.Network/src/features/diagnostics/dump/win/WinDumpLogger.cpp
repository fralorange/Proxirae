#include <string>
#include <Windows.h>

#include "features/diagnostics/dump/win/WinDumpLogger.h"

namespace Proxirae {

    WinDumpLogger::WinDumpLogger()
    {
        std::wstring path(32768, L'\0');

        DWORD length = GetModuleFileNameW(
            nullptr,
            path.data(),
            static_cast<DWORD>(path.size())
        );

        if (length == 0 || length >= path.size())
            return;

        path.resize(length);

        m_logPath = std::filesystem::path(path).parent_path() / L"crash.log";
    }

    void WinDumpLogger::Log(LogLevel level, std::string_view message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_file.is_open())
        {
            if (m_logPath.empty())
                return;

            m_file.open(m_logPath, std::ios::out | std::ios::trunc);

            if (!m_file.is_open())
                return;
        }

        switch (level) {
        case LogLevel::Debug:
            m_file << "[DEBUG] ";
            break;

        case LogLevel::Info:
            m_file << "[INFO] ";
            break;

        case LogLevel::Warning:
            m_file << "[WARNING] ";
            break;

        case LogLevel::Error:
            m_file << "[ERROR] ";
            break;

        case LogLevel::Critical:
            m_file << "[CRITICAL] ";
            break;

        default:
            m_file << "[UNKNOWN] ";
            break;
        }

        m_file << message << '\n';
        m_file.flush();
    }
}