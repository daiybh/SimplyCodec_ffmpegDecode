#pragma once
#include <cstdint>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <stdexcept>
#include <cstring>
#endif

using namespace std::chrono_literals;

namespace os
{
	template <class Rep, class Period>
	void sleep(const std::chrono::duration<Rep, Period>& time)
	{
		std::this_thread::sleep_for(time);
	}

	struct SystemTime
	{
		uint16_t wYear = 0;
		uint16_t wMonth = 0;
		uint16_t wDay = 0;
		uint16_t wHour = 0;
		uint16_t wMinute = 0;
		uint16_t wSecond = 0;
		uint16_t wMilliseconds = 0;
	};

	inline SystemTime getLocalTime()
	{
		SystemTime localTime;

#if _WIN32
		SYSTEMTIME time;
		GetLocalTime(&time);
		localTime.wYear = time.wYear;
		localTime.wMonth = time.wMonth;
		localTime.wDay = time.wDay;
		localTime.wHour = time.wHour;
		localTime.wMinute = time.wMinute;
		localTime.wSecond = time.wSecond;
		localTime.wMilliseconds = time.wMilliseconds;
		return localTime;
#else
		timeval now{};
		gettimeofday(&now, nullptr);
		const time_t nowTime_t = time(nullptr);
		const tm tm = *localtime(&nowTime_t);
		localTime.wYear = tm.tm_year + 1900;
		localTime.wMonth = tm.tm_mon + 1;
		localTime.wDay = tm.tm_mday;
		localTime.wHour = tm.tm_hour;
		localTime.wMinute = tm.tm_min;
		localTime.wSecond = tm.tm_sec;
		localTime.wMilliseconds = now.tv_usec / 1000;
		return localTime;
#endif
	}

#ifndef _WIN32

#define _stricmp strcasecmp
#define _strnicmp strncasecmp

	inline int32_t memcpy_s(void* dst, size_t dstSize, const void* src, size_t srcSize)
	{
		if (srcSize == 0)
			return 0;

		if (dst == nullptr)
			throw std::invalid_argument("dst == nullptr");

		if (src == nullptr || dstSize < srcSize)
		{
			memset(dst, 0, dstSize);

			if (src == nullptr)
				throw std::invalid_argument("src == nullptr");

			if (dstSize < srcSize)
				throw std::invalid_argument("dstSize < srcSize");

			return 1;
		}
		memcpy(dst, src, srcSize);
		return 0;
	}

	//copies count wide characters (two bytes) from src to dest
	inline int32_t wmemcpy_s(wchar_t* dst, size_t dstCount, const wchar_t* src, size_t srcCount)
	{
		if (srcCount == 0)
			return 0;

		if (dst == nullptr)
			throw std::invalid_argument("dst == nullptr");

		if (src == nullptr || dstCount < srcCount)
		{
			wmemset(dst, 0, dstCount);

			if (src == nullptr)
				throw std::invalid_argument("src == nullptr");

			if (dstCount < srcCount)
				throw std::invalid_argument("dstCount < srcCount");

			return 1;
		}
		wmemcpy(dst, src, srcCount);
		return 0;
	}

	inline uint64_t GetTickCount64()
	{
		timespec ts{};
		//This call is not affected by the adjustment of the system time just like GetTickCount() on Windows.
		if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
			return 0;
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
	}
#endif
}
