#pragma once
#include <vector>
#include "RWLocker.h"
#include "lockedPtr.h"

template <class T>
class SyncVector
{
public:

	template <class... Valty>
	void emplace_back(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		m_vector.emplace_back(std::forward<Valty>(val)...);
	}

	bool empty() const
	{
		return m_vector.empty();
	}

	T front()
	{
		SharedLockHolder lock(m_lock);
		return m_vector.front();
	}

	T back()
	{
		SharedLockHolder lock(m_lock);
		return m_vector.back();
	}

	template <class Function>
	decltype(auto) wlock(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		return function(m_vector);
	}

	template <class Function>
	decltype(auto) rlock(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return function(m_vector);
	}

	decltype(auto) rlock()
	{
		return LockedPtr<decltype(m_vector), decltype(m_lock)>(&m_vector, &m_lock, true);
	}

	decltype(auto) wlock()
	{
		return LockedPtr<decltype(m_vector), decltype(m_lock)>(&m_vector, &m_lock, false);
	}

	void clear()
	{
		ExclusiveLockHolder lock(m_lock);
		m_vector.clear();
	}

	size_t size() const
	{
		return m_vector.size();
	}

	decltype(auto) operator[](size_t pos)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_vector[pos];
	}

	decltype(auto) at(size_t pos)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_vector.at(pos);
	}

	template <class Function>
	bool find(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return any_of(begin(m_vector), end(m_vector), [&](const T& obj) { return function(obj); });
	}

	template <class Function>
	void foreach(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		for (const auto &item : m_vector)
		{
			if (function(item))
				break;
		}
	}

private:
	std::vector<T>	m_vector;
	RWLocker		m_lock;
};
