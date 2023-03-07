#pragma once
#include <queue>
#include "RWLocker.h"
#include "lockedPtr.h"

template <class T>
class SyncQueue
{
public:
	void pop()
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_queue.empty())
			return;
		m_queue.pop();
	}

	bool pop_begin(T& val)
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_queue.empty())
			return false;
		val = m_queue.front();
		m_queue.pop();
		return true;
	}

	template <class... Valty>
	void emplace(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		m_queue.emplace(std::forward<Valty>(val)...);
	}

	size_t size() const
	{
		return m_queue.size();
	}

	bool empty() const
	{
		return m_queue.empty();
	}

	void clear()
	{
		ExclusiveLockHolder lock(m_lock);
		while (!m_queue.empty())
			m_queue.pop();
	}

	template <class Function>
	decltype(auto) wlock(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		return function(m_queue);
	}

	template <class Function>
	decltype(auto) rlock(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return function(m_queue);
	}

	decltype(auto) rlock()
	{
		return LockedPtr<decltype(m_queue), decltype(m_lock)>(&m_queue, &m_lock, true);
	}

	decltype(auto) wlock()
	{
		return LockedPtr<decltype(m_queue), decltype(m_lock)>(&m_queue, &m_lock, false);
	}
private:
	std::queue<T>	m_queue;
	RWLocker		m_lock;
};
