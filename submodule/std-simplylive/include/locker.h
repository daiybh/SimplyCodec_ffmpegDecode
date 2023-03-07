#pragma once
#ifdef _WIN32
#include <Windows.h>
#else
#include <mutex>
#endif

class Locker
{
#ifdef _WIN32
	CRITICAL_SECTION m_locker;
#else
	std::recursive_mutex m_locker;
#endif
public:
	Locker()
	{
#ifdef _WIN32
		InitializeCriticalSection(&m_locker);
#endif
	}

	~Locker()
	{
#ifdef _WIN32
		DeleteCriticalSection(&m_locker);
#endif
	}

	Locker& operator=(const Locker&) = delete;
	Locker(const Locker&) = delete;

	void lock()
	{
#ifdef _WIN32
		EnterCriticalSection(&m_locker);
#else
		m_locker.lock();
#endif
	}

	bool trylock()
	{
#ifdef _WIN32
		return TryEnterCriticalSection(&m_locker);
#else
		return m_locker.try_lock();
#endif
	}

	void unlock()
	{
#ifdef _WIN32
		LeaveCriticalSection(&m_locker);
#else
		m_locker.unlock();
#endif
	}
};

class LockHolder
{
public:
	explicit LockHolder(Locker& _locker) : m_locker(_locker)
	{
		m_locker.lock();
	}

	~LockHolder()
	{
		m_locker.unlock();
	}

	LockHolder& operator=(const LockHolder&) = delete;
	LockHolder(const LockHolder&) = delete;

	void lock() const
	{
		m_locker.lock();
	}

	bool trylock() const
	{
		return m_locker.trylock();
	}

	void unlock() const
	{
		m_locker.unlock();
	}
private:
	Locker& m_locker;
};
