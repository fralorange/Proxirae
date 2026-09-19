#pragma once

#include <functional>

#include "IoDatagramResult.h"

namespace Proxirae {
	using IoDatagramCallback = std::function<void(const IoDatagramResult&)>;
}