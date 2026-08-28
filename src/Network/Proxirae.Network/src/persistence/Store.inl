#pragma once

namespace Proxirae {
	template <typename T>
	Store<T>::Store(ItemPtr item)
		: m_current(std::move(item)) {
	}

	template <typename T>
	typename Store<T>::ItemPtr Store<T>::Get() const {
		return m_current.load();
	}

	template <typename T>
	void Store<T>::Update(ItemPtr item)
	{
		m_current.store(std::move(item));
	}
}