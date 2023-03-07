#pragma once
#include <windows.h>

class  CTimeCount
{
	LARGE_INTEGER m_lgFrequency;
	LARGE_INTEGER m_lgStart;
public:
	CTimeCount(void);
	~CTimeCount(void);

	float getCountTime() const;
	void  reset();
};
