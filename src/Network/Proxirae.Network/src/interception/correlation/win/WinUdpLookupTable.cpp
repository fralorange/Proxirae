#include <windows.h>
#include <iphlpapi.h>
#include <cstdint>

#include "interception/correlation/win/WinUdpLookupTable.h"
#include "environment/inet.h"

#pragma comment(lib, "iphlpapi.lib")

namespace Proxirae {
    class WinUdpLookupTable::Snapshot {
    public:
        std::vector<MIB_UDPROW> rows;

        Snapshot(PMIB_UDPTABLE table) {
            if (table && table->dwNumEntries > 0) {
                rows.assign(table->table, table->table + table->dwNumEntries);
            }
        }
    };

    std::optional<WinUdpLookupTable> WinUdpLookupTable::TryCreate()
    {
        DWORD size = 0;

        if (GetUdpTable(nullptr, &size, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
            std::vector<std::byte> buffer(size);
            auto pUdpTable = reinterpret_cast<PMIB_UDPTABLE>(buffer.data());

            if (GetUdpTable(pUdpTable, &size, TRUE) == NO_ERROR) {
                auto snapshot = std::make_unique<Snapshot>(pUdpTable);
                return WinUdpLookupTable(std::move(snapshot));
            }
        }

        return std::nullopt;
    }

    WinUdpLookupTable::WinUdpLookupTable(std::unique_ptr<Snapshot> snapshot)
        : m_snapshot(std::move(snapshot)) {
    }

    WinUdpLookupTable::WinUdpLookupTable(WinUdpLookupTable&&) noexcept = default;
    WinUdpLookupTable::~WinUdpLookupTable() = default;

    bool WinUdpLookupTable::CanAssociate(const FiveTuple& bindKey) const
    {
        if (!m_snapshot) {
            return false;
        }

        for (const auto& row : m_snapshot->rows) {

            std::uint16_t rowPort = ntohs(static_cast<std::uint16_t>(row.dwLocalPort));
            std::uint32_t rowAddr = ntohl(row.dwLocalAddr);

            if (rowPort == bindKey.srcPort) {
                if (rowAddr == bindKey.srcAddress || rowAddr == INADDR_ANY) {
                    return true;
                }
            }
        }

        return false;
    }

    bool WinUdpLookupTable::IsEmpty() const
    {
        return !m_snapshot || m_snapshot->rows.empty();
    }

    bool WinUdpLookupTable::TryRemoveBind(const FiveTuple& bindKey) const
    {
        if (IsEmpty()) {
            return false;
        }

        auto& rows = m_snapshot->rows;

        for (size_t i = 0; i < rows.size(); ++i) {
            std::uint16_t rowPort = ntohs(static_cast<std::uint16_t>(rows[i].dwLocalPort));
            std::uint32_t rowAddr = ntohl(rows[i].dwLocalAddr);

            if (rowPort == bindKey.srcPort) {
                if (rowAddr == bindKey.srcAddress || rowAddr == INADDR_ANY) {
                    rows[i] = rows.back();
                    rows.pop_back();

                    return true;
                }
            }
        }

        return false; 
    }
}