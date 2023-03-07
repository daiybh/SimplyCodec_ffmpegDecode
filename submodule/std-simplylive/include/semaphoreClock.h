#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <semaphore.h>
#include <climits>
#endif

class SemaphoreClock
{
public:
	SemaphoreClock()
	{
#ifdef _WIN32
		m_handle = ::CreateSemaphore(nullptr, 0, LONG_MAX, nullptr);
#else
		sem_init(&m_handle, 0, INT_MAX);
#endif
	}

	SemaphoreClock(const char* name)
	{
#ifdef _WIN32
		m_handle = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, TRUE, name);
		if (m_handle == nullptr)
			m_handle = ::CreateSemaphoreA(nullptr, 0, LONG_MAX, name);
#else
		sem_init(&m_handle, 0, INT_MAX);
#endif
	}

	~SemaphoreClock()
	{
#ifdef _WIN32
		CloseHandle(m_handle);
#else
		sem_destroy(&m_handle);
#endif
	}

	void raiseEvent()
	{
#ifdef _WIN32
		ReleaseSemaphore(m_handle, 1, nullptr);
#else
		sem_post(&m_handle);
#endif
	}
	bool waitEvent(int nTimeOut = 50)
	{
#ifdef _WIN32
		return WaitForSingleObject(m_handle, nTimeOut) == WAIT_OBJECT_0;
#else
		return sem_wait(&m_handle) == 0;//here we don't have time
#endif
	}

private:
#ifdef _WIN32
	void* m_handle = nullptr;
#else
	sem_t  m_handle;
#endif
};
