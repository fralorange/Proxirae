#pragma once

#include <functional>

#include "io/IoResult.h"

namespace Proxirae {
	using IoCallback = std::function<void(const IoResult&)>;
}