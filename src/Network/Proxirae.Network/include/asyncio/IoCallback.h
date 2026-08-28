#pragma once

#include <functional>

#include "asyncio/IoResult.h"

namespace Proxirae {
	using IoCallback = std::function<void(const IoResult&)>;
}