#pragma once
#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

using cpuFreq = uint64_t;
using cpuTick = uint64_t;

inline void getCPUfreq(cpuFreq& _freq)
{
#ifdef _WIN32
	QueryPerformanceFrequency((LARGE_INTEGER*)&_freq);
#else
	_freq = 1000000;
#endif
}

inline void getTickCount(cpuTick& _tick)
{
#ifdef _WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&_tick);
#else
	timeval now{};
	gettimeofday(&now, nullptr);
	_tick = now.tv_sec * 1000000 + now.tv_usec;//change to microsecond
#endif
}

inline cpuTick getTickCount()
{
	cpuTick ret;
	getTickCount(ret);
	return ret;
}
