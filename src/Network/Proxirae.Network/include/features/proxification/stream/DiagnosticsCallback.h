#pragma once

#include <functional>
#include <cstdint>

#include "features/proxification/stream/TestStage.h"

namespace Proxirae {
    using DiagnosticsCallback = std::function<void(TestStage stage, std::uint64_t latencyMs, bool success)>;
}