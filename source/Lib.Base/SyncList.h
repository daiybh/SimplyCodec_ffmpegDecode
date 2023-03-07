#pragma once
#include <list>
#include <functional>
#include "locker.h"
using namespace std;

template<class T>
class SyncList
{
	list<T> m_list;
	Locker	m_lock;

public:
	SyncList() = default;

	~SyncList() = default;

	void emplace_back(const T& _val)
	{
		LockHolder lock(m_lock);
		m_list.emplace_back(_val);
	}

	void emplace_front(const T& _val)
	{
		LockHolder lock(m_lock);
		m_list.emplace_front(_val);
	}

	void pop_front()
	{
		LockHolder lock(m_lock);
		m_list.pop_front();
	}

	bool pop_begin(T& _val)
	{
		LockHolder lock(m_lock);
		if (m_list.empty())
			return false;
		_val = m_list.front();
		m_list.pop_front();
		return true;
	}

	void pop_back()
	{
		LockHolder lock(m_lock);
		m_list.pop_back();
	}

	T front()
	{
		LockHolder lock(m_lock);
		return m_list.front();
	}

	uint32_t size()
	{
		return m_list.size();
	}

	bool empty()
	{
		return m_list.empty();
	}

	void clear()
	{
		LockHolder lock(m_lock);
		return m_list.clear();
	}

	bool find(const T& item)
	{
		LockHolder lock(m_lock);
		return any_of(begin(m_list), end(m_list), [&](const T& obj) { return item == obj; });
	}

	bool find(function<bool(const T&)> predicate)
	{
		LockHolder lock(m_lock);
		return any_of(begin(m_list), end(m_list), [&](const T& obj) { return predicate(obj); });
	}

	bool remove(const T& item)
	{
		LockHolder lock(m_lock);
		m_list.remove(item);
		return true;
	}

	void for_each(function<bool(const T&)> func = nullptr)
	{
		if (func == nullptr) return;
		LockHolder lock(m_lock);
		for (const auto& item : m_list)
		{
			if (!func(item))
				break;
		}
	}

	list<T> getCopy()
	{
		LockHolder lock(m_lock);
		return m_list;
	}

	explicit SyncList(SyncList<T>& other)
	{
		LockHolder lock(m_lock);
		m_list.clear();
		other.for_each([&](const auto& item)
		{
			m_list.emplace_back(item);
			return true;
		});
	}

	SyncList<T>& operator=(SyncList<T>& other)
	{
		if (this == &other)
			return *this;
		LockHolder lock(m_lock);
		m_list.clear();
		other.for_each([&](const auto& item)
		{
			m_list.emplace_back(item);
			return true;
		});
		return *this;
	}
};
