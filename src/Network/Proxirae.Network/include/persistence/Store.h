#pragma once

#include <memory>

namespace Proxirae {
	template <typename T>
	class Store {
	public:
		using ItemPtr = std::shared_ptr<const T>;

		explicit Store(ItemPtr item);

		ItemPtr Get() const;
		void Update(ItemPtr item);

	private:
		std::atomic<ItemPtr> m_current;
	};
}

#include "persistence/Store.inl"