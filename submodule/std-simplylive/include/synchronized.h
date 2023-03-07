#pragma once
#include "RWLocker.h"
#include "lockedPtr.h"

/**
 *Usage Example:
	std::list<int> temp = { 0,1,2,3,45 };
	Synchronized<std::list<int>> m_container;
	m_container.wlock([&](auto& item)
		{
			std::swap(item, temp);
		});
	size_t size = m_container.rlock([&](auto& item)
		{
			return item.size();
		});
	size_t size2 = m_container.unsafe()->size();
	m_container.wlock()->emplace_back(1);
	m_container.wlock()->remove(1);
 */
//https://github.com/facebook/folly/blob/master/folly/Synchronized.h

template <class T>
class Synchronized
{
public:

	template <class Function>
	decltype(auto) rlock(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return function(m_datum);
	}

	template <class Function>
	decltype(auto) wlock(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		return function(m_datum);
	}

	decltype(auto) rlock()
	{
		return LockedPtr<decltype(m_datum), decltype(m_lock)>(&m_datum, &m_lock, true);
	}

	decltype(auto) wlock()
	{
		return LockedPtr<decltype(m_datum), decltype(m_lock)>(&m_datum, &m_lock, false);
	}

	T& unsafe()
	{
		return m_datum;
	}

private:
	RWLocker m_lock;
	T        m_datum;
};
