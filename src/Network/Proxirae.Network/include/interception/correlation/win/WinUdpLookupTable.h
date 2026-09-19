#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <cstddef>

#include "primitives/tuples/FiveTuple.h"

namespace Proxirae {
    class WinUdpLookupTable {
    private:
        class Snapshot;
        std::unique_ptr<Snapshot> m_snapshot;

    public:
        static std::optional<WinUdpLookupTable> TryCreate();

        WinUdpLookupTable(WinUdpLookupTable&&) noexcept;
        ~WinUdpLookupTable();

        bool CanAssociate(const FiveTuple& bindKey) const;
        bool IsEmpty() const;
        
        bool TryRemoveBind(const FiveTuple& bindKey) const;
    private:
        WinUdpLookupTable(std::unique_ptr<Snapshot> snapshot);
    };
}