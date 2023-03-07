#pragma once
#include <Windows.h>

class Locker
{
	CRITICAL_SECTION	 m_locker;

public:
	Locker();
	~Locker();
	Locker& operator=(const Locker&) = delete;
	Locker(const Locker &) = delete;

	void lock();
	bool try_lock();
	void unlock();
};

class LockHolder
{
	Locker& m_locker;

public:
	explicit LockHolder(Locker& _locker);
	~LockHolder();
	LockHolder& operator=(const LockHolder&) = delete;
	LockHolder(const LockHolder &) = delete;

	void lock() const;
	bool try_lock() const;
	void unlock() const;
};
