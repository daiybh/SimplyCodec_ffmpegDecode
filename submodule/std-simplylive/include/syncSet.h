#pragma once
#include <set>
#include "RWLocker.h"
#include "lockedPtr.h"

template <class K, class Pr = std::less<K>>
class SyncSet
{
public:

	template <class... Valty>
	bool emplace(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_set.emplace(std::forward<Valty>(val)...).second;
	}

	bool empty()
	{
		return m_set.empty();
	}

	size_t size() const
	{
		return m_set.size();
	}

	bool find(const K& key)
	{
		SharedLockHolder lock(m_lock);
		return m_set.contains(key);
	}

	void clear()
	{
		ExclusiveLockHolder lock(m_lock);
		m_set.clear();
	}

	void erase(const K& key)
	{
		ExclusiveLockHolder lock(m_lock);
		m_set.erase(key);
	}

	template <class Function>
	decltype(auto) rlock(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return function(m_set);
	}

	template <class Function>
	decltype(auto) wlock(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		return function(m_set);
	}

	decltype(auto) rlock()
	{
		return LockedPtr<decltype(m_set), decltype(m_lock)>(&m_set, &m_lock, true);
	}

	decltype(auto) wlock()
	{
		return LockedPtr<decltype(m_set), decltype(m_lock)>(&m_set, &m_lock, false);
	}

	template <class Function>
	void foreach(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		for (const auto& item : m_set)
		{
			if (function(item))
				break;
		}
	}
private:
	std::set<K, Pr>	m_set;
	RWLocker		m_lock;
};
