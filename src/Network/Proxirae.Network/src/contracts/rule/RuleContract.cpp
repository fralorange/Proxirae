#include "contracts/rule/RuleContract.h"

namespace Proxirae {
	bool RuleContract::operator<(const RuleContract& other) const
	{
		return priority < other.priority;
	}
}