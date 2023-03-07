#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <mutex>
#endif

class RWLocker
{
#ifdef _WIN32
	SRWLOCK m_locker;
#else
	std::shared_mutex m_locker;
#endif
public:
	RWLocker()
	{
#ifdef _WIN32
		InitializeSRWLock(&m_locker);
#endif
	}

	~RWLocker() = default;

	RWLocker& operator=(const RWLocker&) = delete;
	RWLocker(const RWLocker&) = delete;

	void lockShared()
	{
#ifdef _WIN32
		AcquireSRWLockShared(&m_locker);
#else
		m_locker.lock_shared();
#endif
	}

	bool tryLockShared()
	{
#ifdef _WIN32
		return TryAcquireSRWLockShared(&m_locker);
#else
		m_locker.try_lock_shared();
#endif
	}

	void unlockShared()
	{
#ifdef _WIN32
		ReleaseSRWLockShared(&m_locker);
#else
		m_locker.unlock_shared();
#endif
	}

	void lockExclusive()
	{
#ifdef _WIN32
		AcquireSRWLockExclusive(&m_locker);
#else
		m_locker.lock();
#endif
	}

	bool tryLockExclusive()
	{
#ifdef _WIN32
		return TryAcquireSRWLockExclusive(&m_locker);
#else
		m_locker.try_lock();
#endif
	}

	void unlockExclusive()
	{
#ifdef _WIN32
		ReleaseSRWLockExclusive(&m_locker);
#else
		m_locker.unlock();
#endif
	}
};

class SharedLockHolder
{
public:
	explicit SharedLockHolder(RWLocker& _locker) : m_locker(_locker)
	{
		m_locker.lockShared();
	}

	explicit SharedLockHolder(RWLocker* _locker) : m_locker(*_locker)
	{
		m_locker.lockShared();
	}

	~SharedLockHolder()
	{
		m_locker.unlockShared();
	}

	SharedLockHolder& operator=(const SharedLockHolder&) = delete;
	SharedLockHolder(const SharedLockHolder&) = delete;

	void lock() const
	{
		m_locker.lockShared();
	}

	bool try_lock() const
	{
		return m_locker.tryLockShared();
	}

	void unlock() const
	{
		m_locker.unlockShared();
	}
private:
	RWLocker& m_locker;
};

class ExclusiveLockHolder
{
public:
	explicit ExclusiveLockHolder(RWLocker& _locker) : m_locker(_locker)
	{
		m_locker.lockExclusive();
	}

	explicit ExclusiveLockHolder(RWLocker* _locker) : m_locker(*_locker)
	{
		m_locker.lockExclusive();
	}

	~ExclusiveLockHolder()
	{
		m_locker.unlockExclusive();
	}

	ExclusiveLockHolder& operator=(const ExclusiveLockHolder&) = delete;
	ExclusiveLockHolder(const ExclusiveLockHolder&) = delete;

	void lock() const
	{
		m_locker.lockExclusive();
	}

	bool try_lock() const
	{
		return m_locker.tryLockExclusive();
	}

	void unlock() const
	{
		m_locker.unlockExclusive();
	}
private:
	RWLocker& m_locker;
};