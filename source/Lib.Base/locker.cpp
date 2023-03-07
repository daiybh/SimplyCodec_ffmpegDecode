#include "locker.h"

Locker::Locker()
{
	//InitializeCriticalSectionAndSpinCount(&m_locker, 4000);
	InitializeCriticalSection(&m_locker);
}

Locker::~Locker()
{
	DeleteCriticalSection(&m_locker);
}

void Locker::lock()
{
	EnterCriticalSection(&m_locker);
}

bool Locker::try_lock()
{
	return TryEnterCriticalSection(&m_locker);
}

void Locker::unlock()
{
	LeaveCriticalSection(&m_locker);
}

LockHolder::LockHolder(Locker& _locker) : m_locker(_locker)
{
	m_locker.lock();
}

LockHolder::~LockHolder()
{
	m_locker.unlock();
}

void LockHolder::lock() const
{
	m_locker.lock();
}

bool LockHolder::try_lock() const
{
	return m_locker.try_lock();
}

void LockHolder::unlock() const
{
	m_locker.unlock();
}