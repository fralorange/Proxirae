#pragma once

#include <functional>

#include "IoResult.h"

namespace Proxirae {
	using IoCallback = std::function<void(const IoResult&)>;
}