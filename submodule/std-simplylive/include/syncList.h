#pragma once
#include <list>
#include "RWLocker.h"
#include "lockedPtr.h"

template <class T>
class SyncList
{
public:
	template <class... Valty>
	void emplace_back(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		m_list.emplace_back(std::forward<Valty>(val)...);
	}

	template <class... Valty>
	void emplace_front(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		m_list.emplace_front(std::forward<Valty>(val)...);
	}

	template <class Function>
	decltype(auto) wlock(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		return function(m_list);
	}

	template <class Function>
	decltype(auto) rlock(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return function(m_list);
	}

	decltype(auto) rlock()
	{
		return LockedPtr<decltype(m_list), decltype(m_lock)>(&m_list, &m_lock, true);
	}

	decltype(auto) wlock()
	{
		return LockedPtr<decltype(m_list), decltype(m_lock)>(&m_list, &m_lock, false);
	}

	void pop_front()
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_list.empty())
			return;
		m_list.pop_front();
	}

	bool pop_begin(T& _val)
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_list.empty())
			return false;
		_val = m_list.front();
		m_list.pop_front();
		return true;
	}

	void pop_back()
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_list.empty())
			return;
		m_list.pop_back();
	}

	T front()
	{
		SharedLockHolder lock(m_lock);
		return m_list.front();
	}

	T back()
	{
		SharedLockHolder lock(m_lock);
		return m_list.back();
	}

	size_t size() const
	{
		return m_list.size();
	}

	bool empty() const
	{
		return m_list.empty();
	}

	void clear()
	{
		ExclusiveLockHolder lock(m_lock);
		return m_list.clear();
	}

	bool exist(const T& item)
	{
		SharedLockHolder lock(m_lock);
		return any_of(begin(m_list), end(m_list), [&](const T& obj) { return item == obj; });
	}

	template <class Function>
	bool find(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return any_of(begin(m_list), end(m_list), [&](const T& obj) { return function(obj); });
	}

	bool remove(const T& item)
	{
		ExclusiveLockHolder lock(m_lock);
		m_list.remove(item);
		return true;
	}

	template <class Function>
	void foreach(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		for (const auto &item : m_list)
		{
			if (function(item))
				break;
		}
	}
private:
	std::list<T>	m_list;
	RWLocker		m_lock;
};
