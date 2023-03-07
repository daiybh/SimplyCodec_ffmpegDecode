#pragma once
#include <map>
#include "RWLocker.h"
#include "lockedPtr.h"

template <class K, class V, class Pr = std::less<K>>
class SyncMap
{
public:
	bool pop_front()
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_map.empty())
			return false;
		m_map.erase(m_map.begin());
		return true;
	}

	bool pop_begin(V& val)
	{
		ExclusiveLockHolder lock(m_lock);
		if (m_map.empty())
			return false;
		auto iter = m_map.begin();
		val = iter->second;
		m_map.erase(iter);
		return true;
	}

	bool emplace(const std::pair<K, V>& val)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_map.emplace(val).second;
	}

	template <class... Valty>
	bool emplace(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_map.emplace(std::forward<Valty>(val)...).second;
	}

	template <class... Valty>
	bool try_emplace(Valty&&...val)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_map.try_emplace(std::forward<Valty>(val)...).second;
	}

	void erase(const K& key)
	{
		ExclusiveLockHolder lock(m_lock);
		m_map.erase(key);
	}

	bool insert_or_assign(const K& key, const V& value)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_map.insert_or_assign(key, value).second;
	}

	bool empty() const
	{
		return m_map.empty();
	}

	size_t size() const
	{
		return m_map.size();
	}

	void clear()
	{
		ExclusiveLockHolder lock(m_lock);
		m_map.clear();
	}

	bool find(const K& key)
	{
		SharedLockHolder lock(m_lock);
		return m_map.contains(key);
	}

	bool find(const K& key, V& value)
	{
		SharedLockHolder lock(m_lock);
		auto item = m_map.find(key);
		if (item == m_map.end())
			return false;
		value = item->second;
		return true;
	}

	decltype(auto) operator[](const K& keyval)
	{
		ExclusiveLockHolder lock(m_lock);
		return m_map[keyval];
	}

	template <class Function>
	decltype(auto) wlock(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		return function(m_map);
	}

	template <class Function>
	decltype(auto) rlock(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		return function(m_map);
	}

	decltype(auto) rlock()
	{
		return LockedPtr<decltype(m_map), decltype(m_lock)>(&m_map, &m_lock, true);
	}

	decltype(auto) wlock()
	{
		return LockedPtr<decltype(m_map), decltype(m_lock)>(&m_map, &m_lock, false);
	}

	template <class Function>
	void remove_if(Function&& function)
	{
		ExclusiveLockHolder lock(m_lock);
		for (auto iter = m_map.begin(); iter != m_map.end();)
		{
			if (function(iter->first, iter->second))
				iter = m_map.erase(iter);
			else
				++iter;
		}
	}

	template <class Function>
	void foreach(Function&& function)
	{
		SharedLockHolder lock(m_lock);
		for (const auto& item : m_map)
		{
			if (function(item.first, item.second))
				break;
		}
	}

private:
	std::map<K, V, Pr>	m_map;
	RWLocker			m_lock;
};
