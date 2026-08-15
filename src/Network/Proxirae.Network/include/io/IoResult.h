#pragma once

#include <cstddef>

namespace Proxirae {
	struct IoResult {
		bool success;
		std::size_t bytesTransferred;
		int errorCode;
	};
}