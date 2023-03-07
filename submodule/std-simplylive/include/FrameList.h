#pragma once
#include "locker.h"
#include <list>
#include "semaphoreClock.h"

template <class T>
class FrameList
{
public:

	bool popFrontFromEmptyList(T& _elem)
	{
		LockHolder locker(m_emptyListLock);
		if (m_emptyList.empty())
			return false;

		_elem = m_emptyList.front();
		m_emptyList.pop_front();
		return true;
	}

	bool popFrontFromDataList(T& _elem)
	{
		LockHolder locker(m_dataListLock);
		if (m_dataList.empty())
			return false;

		_elem = m_dataList.front();
		m_dataList.pop_front();
		return true;
	}

	bool popEndFromDataList(T& _elem)
	{
		LockHolder locker(m_dataListLock);
		if (m_dataList.empty())
			return false;

		_elem = m_dataList.back();
		m_dataList.pop_back();
		return true;
	}

	void pushBackToDataList(T& _elem)
	{
		LockHolder locker(m_dataListLock);
		m_dataList.emplace_back(_elem);
		m_dataListSemaphore.raiseEvent();
	}

	void pushBackToDataList(T&& _elem)
	{
		LockHolder locker(m_dataListLock);
		m_dataList.emplace_back(std::forward<T>(_elem));
		m_dataListSemaphore.raiseEvent();
	}

	void pushFrontToDataList(T& _elem)
	{
		LockHolder locker(m_dataListLock);
		m_dataList.emplace_front(_elem);
		m_dataListSemaphore.raiseEvent();
	}

	void pushFrontToDataList(T&& _elem)
	{
		LockHolder locker(m_dataListLock);
		m_dataList.emplace_front(std::forward<T>(_elem));
		m_dataListSemaphore.raiseEvent();
	}

	void pushBackToEmptyList(T& _elem)
	{
		LockHolder locker(m_emptyListLock);
		m_emptyList.emplace_back(_elem);
		m_emptyListSemaphore.raiseEvent();
	}

	void pushBackToEmptyList(T&& _elem)
	{
		LockHolder locker(m_emptyListLock);
		m_emptyList.emplace_back(std::forward<T>(_elem));
		m_emptyListSemaphore.raiseEvent();
	}

	bool waitForDataList(int32_t timeout = 100)
	{
		return m_dataListSemaphore.waitEvent(timeout);
	}

	bool waitForEmptyList(int32_t timeout = 100)
	{
		return m_emptyListSemaphore.waitEvent(timeout);
	}

	size_t sizeOfEmptyList()
	{
		return m_emptyList.size();
	}

	size_t sizeOfDataList()
	{
		return m_dataList.size();
	}

protected:
	std::list<T>   m_emptyList;
	std::list<T>   m_dataList;
	Locker         m_emptyListLock;
	Locker         m_dataListLock;
	SemaphoreClock m_emptyListSemaphore;
	SemaphoreClock m_dataListSemaphore;
};
