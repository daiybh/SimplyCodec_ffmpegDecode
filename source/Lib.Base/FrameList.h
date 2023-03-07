#pragma once
#include "locker.h"
#include <list>
#include <memory>
using namespace std;

template <class T>
class FrameList
{
protected:
	list<T *> m_emptyList;
	list<T *> m_dataList;
	Locker m_emptyListLock;
	Locker m_dataListLock;

	template<typename X, typename... Args>
	void addOneFree(Args&&... args)
	{
		LockHolder locker(m_emptyListLock);

		T* p = new X(std::forward<Args>(args)...);
		if (p != nullptr)
			m_emptyList.push_back(p);
	}

	template<typename X, typename... Args>
	void setMaxSize(int _size, Args&&... args)
	{
		for (int i = 0; i < _size; i++)
			addOneFree<X>(std::forward<Args>(args)...);
	}
public:
	FrameList() {}
	~FrameList() {}

	template<typename X, typename... Args>
	void initialize(int _size, Args&&... args)
	{
		setMaxSize<X>(_size, std::forward<Args>(args)...);
	}

	bool pop_front_empty(T* & _elem)
	{
		LockHolder locker(m_emptyListLock);

		if (!m_emptyList.empty())
		{
			auto it = m_emptyList.begin();
			_elem = *it;
			m_emptyList.pop_front();
			return true;
		}
		_elem = nullptr;
		return false;
	}

	bool pop_front_data(T* & _elem)
	{
		LockHolder locker(m_dataListLock);

		if (!m_dataList.empty())
		{
			auto it = m_dataList.begin();
			_elem = *it;
			m_dataList.pop_front();
			return true;
		}
		_elem = nullptr;
		return false;
	}

	bool getHeadDataRef(T* & _elem)
	{
		LockHolder locker(m_dataListLock);

		if (!m_dataList.empty())
		{
			auto it = m_dataList.begin();
			_elem = *it;
			return true;
		}
		_elem = nullptr;
		return false;
	}

	void push_back_data(T* _elem)
	{
		LockHolder locker(m_dataListLock);

		if (_elem != nullptr)
			m_dataList.push_back(_elem);
	}

	void push_front_data(T* _elem)
	{
		LockHolder locker(m_dataListLock);

		if (_elem != nullptr)
			m_dataList.push_front(_elem);
	}

	void push_back_empty(T* _elem)
	{
		LockHolder locker(m_emptyListLock);

		if (_elem != nullptr)
			m_emptyList.push_back(_elem);
	}

	uint32_t get_size_empty()
	{
		return m_emptyList.size();
	}

	uint32_t get_size_data()
	{
		return m_dataList.size();
	}
};
