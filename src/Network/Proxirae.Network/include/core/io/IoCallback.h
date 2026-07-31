#pragma once

#include <functional>

#include "core/io/IoResult.h"

namespace Proxirae {
	using IoCallback = std::function<void(const IoResult&)>;
}