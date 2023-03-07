#include "SemaphoreClock.h"

SemaphoreClock::SemaphoreClock() : m_handle(nullptr)
{
	m_handle = ::CreateSemaphore(nullptr, 0, LONG_MAX, nullptr);
}

SemaphoreClock::~SemaphoreClock()
{
	CloseHandle(m_handle);
}

void SemaphoreClock::raiseEvent() const
{
	ReleaseSemaphore(m_handle, 1, nullptr);
}

bool SemaphoreClock::waitEvent(int nTimeCout) const
{
	if (WaitForSingleObject(m_handle, nTimeCout) == WAIT_OBJECT_0)
		return true;
	return false;
}