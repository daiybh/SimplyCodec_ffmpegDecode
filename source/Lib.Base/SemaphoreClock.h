#pragma once
#include <Windows.h>

class SemaphoreClock
{
	HANDLE m_handle;
public:
	SemaphoreClock();
	~SemaphoreClock();

	void raiseEvent() const;
	bool waitEvent(int nTimeCout) const;
};
