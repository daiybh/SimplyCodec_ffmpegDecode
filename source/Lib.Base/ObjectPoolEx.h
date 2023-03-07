#pragma once
#include "locker.h"
#include <list>
#include <memory>
#include "SemaphoreClock.h"
using namespace std;

template <class T>
class ObjectPoolEx
{
	list<T *> m_data_list;
	Locker m_mutex;
	SemaphoreClock m_DataSemaphoreClock;

	template<typename X, typename... Args>
	void addOneFree(Args&&... args)
	{
		T* p = new X(std::forward<Args>(args)...);
		if (p != nullptr)
		{
			m_data_list.push_back(p);
			m_DataSemaphoreClock.raiseEvent();
		}
	}

	template<typename X, typename... Args>
	void SetMaxSize(int _size, Args&&... args)
	{
		LockHolder locker(m_mutex);

		for (int i = 0; i < _size; i++)
			addOneFree<X>(std::forward<Args>(args)...);
	}
public:
	ObjectPoolEx() {}
	~ObjectPoolEx() {}

	template<typename X, typename... Args>
	void initialize(int _size, Args&&... args)
	{
		SetMaxSize<X>(_size, std::forward<Args>(args)...);
	}

	bool getNew(T* & _elem)
	{
		LockHolder locker(m_mutex);

		if (!m_data_list.empty())
		{
			auto it = m_data_list.begin();
			_elem = *it;
			m_data_list.pop_front();
			return true;
		}
		_elem = nullptr;
		return false;
	}
	bool waitDataEvent(int nTimeout) {
		return m_DataSemaphoreClock.waitEvent(nTimeout);
	}
	void release(T* _elem)
	{
		LockHolder locker(m_mutex);

		if (_elem != nullptr)
		{
			m_data_list.push_back(_elem);
		}
		m_DataSemaphoreClock.raiseEvent();
	}

	uint32_t size()
	{
		return m_data_list.size();
	}

	//shared_ptr interface
	static ObjectPoolEx<T>& getInstance()
	{
		static ObjectPoolEx<T> ob_pool;
		return ob_pool;
	}

	static T *newObject()
	{
		T* obj = nullptr;
		ObjectPoolEx<T>::getInstance().getNew(obj);
		return obj;
	}

	static void deleteObject(T* p)
	{
		ObjectPoolEx<T>::getInstance().release(p);
	}

	static shared_ptr<T> Get()
	{
		shared_ptr<T> obj(ObjectPoolEx<T>::newObject(), deleteObject);
		return obj;
	}

	static uint32_t getSize()
	{
		return ObjectPoolEx<T>::getInstance().size();
	}
};
