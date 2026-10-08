#include "platform/environment/inet.h"

#include <windows.h>
#include <iphlpapi.h>
#include <cstdint>
#include <cstddef>

#include "features/interception/correlation/win/WinUdpLookupTable.h"

#pragma comment(lib, "iphlpapi.lib")

namespace Proxirae {
    class WinUdpLookupTable::Snapshot {
    public:
        std::vector<MIB_UDPROW> ipv4Rows;
        std::vector<MIB_UDP6ROW> ipv6Rows;

        Snapshot(PMIB_UDPTABLE table4, PMIB_UDP6TABLE table6) {
            if (table4 && table4->dwNumEntries > 0) {
                ipv4Rows.assign(table4->table, table4->table + table4->dwNumEntries);
            }
            if (table6 && table6->dwNumEntries > 0) {
                ipv6Rows.assign(table6->table, table6->table + table6->dwNumEntries);
            }
        }
    };

    std::optional<WinUdpLookupTable> WinUdpLookupTable::TryCreate()
    {
        DWORD size4 = 0;
        PMIB_UDPTABLE table4 = nullptr;
        std::vector<std::byte> buffer4;

        if (GetExtendedUdpTable(nullptr, &size4, FALSE, AF_INET, UDP_TABLE_BASIC, 0) == ERROR_INSUFFICIENT_BUFFER) {
            buffer4.resize(size4);
            if (GetExtendedUdpTable(buffer4.data(), &size4, FALSE, AF_INET, UDP_TABLE_BASIC, 0) == NO_ERROR) {
                table4 = reinterpret_cast<PMIB_UDPTABLE>(buffer4.data());
            }
        }

        DWORD size6 = 0;
        PMIB_UDP6TABLE table6 = nullptr;
        std::vector<std::byte> buffer6;

        if (GetExtendedUdpTable(nullptr, &size6, FALSE, AF_INET6, UDP_TABLE_BASIC, 0) == ERROR_INSUFFICIENT_BUFFER) {
            buffer6.resize(size6);
            if (GetExtendedUdpTable(buffer6.data(), &size6, FALSE, AF_INET6, UDP_TABLE_BASIC, 0) == NO_ERROR) {
                table6 = reinterpret_cast<PMIB_UDP6TABLE>(buffer6.data());
            }
        }

        if (table4 || table6) {
            auto snapshot = std::make_unique<Snapshot>(table4, table6);
            return WinUdpLookupTable(std::move(snapshot));
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

        if (!bindKey.srcAddress.isIPv6) {
            for (const auto& row : m_snapshot->ipv4Rows) {
                std::uint16_t rowPort = ntohs(static_cast<std::uint16_t>(row.dwLocalPort));

                if (rowPort == bindKey.srcPort) {
                    if (row.dwLocalAddr == bindKey.srcAddress.data[0] || row.dwLocalAddr == 0) {
                        return true;
                    }
                }
            }
        }
        else {
            for (const auto& row : m_snapshot->ipv6Rows) {
                std::uint16_t rowPort = ntohs(static_cast<std::uint16_t>(row.dwLocalPort));

                if (rowPort == bindKey.srcPort) {
                    bool isAnyAddr = true;

                    for (int i = 0; i < 16; ++i) {
                        if (row.dwLocalAddr.u.Byte[i] != 0) {
                            isAnyAddr = false;
                            break;
                        }
                    }

                    if (isAnyAddr ||
                        std::memcmp(
                            &row.dwLocalAddr,
                            bindKey.srcAddress.data.data(),
                            16
                        ) == 0) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool WinUdpLookupTable::IsEmpty() const
    {
        return !m_snapshot || (m_snapshot->ipv4Rows.empty() && m_snapshot->ipv6Rows.empty());
    }

    bool WinUdpLookupTable::TryRemoveBind(const FiveTuple& bindKey) const
    {
        if (IsEmpty()) {
            return false;
        }

        if (!bindKey.srcAddress.isIPv6) {
            auto& rows = m_snapshot->ipv4Rows;
            
            for (std::size_t i = 0; i < rows.size(); ++i) {
                std::uint16_t rowPort = ntohs(static_cast<std::uint16_t>(rows[i].dwLocalPort));

                if (rowPort == bindKey.srcPort) {
                    if (rows[i].dwLocalAddr == bindKey.srcAddress.data[0] || rows[i].dwLocalAddr == 0) {
                        rows[i] = rows.back();
                        rows.pop_back();

                        return true;
                    }
                }
            }
        }
        else {
            auto& rows = m_snapshot->ipv6Rows;

            for (std::size_t i = 0; i < rows.size(); ++i) {
                std::uint16_t rowPort = ntohs(static_cast<std::uint16_t>(rows[i].dwLocalPort));

                if (rowPort == bindKey.srcPort) {
                    bool isAnyAddr = true;
                    for (int j = 0; j < 16; ++j) {
                        if (rows[i].dwLocalAddr.u.Byte[j] != 0) {
                            isAnyAddr = false;
                            break;
                        }
                    }

                    if (isAnyAddr || std::memcmp(&rows[i].dwLocalAddr, bindKey.srcAddress.data.data(), 16) == 0) {
                        rows[i] = rows.back();
                        rows.pop_back();
                        return true;
                    }
                }
            }
        }

        return false;
    }
}