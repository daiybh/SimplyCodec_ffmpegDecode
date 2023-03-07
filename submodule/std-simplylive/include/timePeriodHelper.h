#pragma once

#ifdef _WIN32

#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

#else
#define timeBeginPeriod
#define timeEndPeriod
#endif
class TimePeriodHelper
{
public:
	TimePeriodHelper() { timeBeginPeriod(1); }
	~TimePeriodHelper() { timeEndPeriod(1); }

	TimePeriodHelper& operator=(const TimePeriodHelper&) = delete;
	TimePeriodHelper(const TimePeriodHelper&) = delete;

	TimePeriodHelper& operator=(TimePeriodHelper&&) = delete;
	TimePeriodHelper(TimePeriodHelper&&) = delete;
};

