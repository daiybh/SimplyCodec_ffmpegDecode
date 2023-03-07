#pragma once
#include <thread>
#include <string>
#include "stringHelper.h"

#ifdef _WIN32
#include <Windows.h>
#include <processthreadsapi.h>
#else
#include <pthread.h>
#endif

struct threadHandle
{
	std::thread* handle = nullptr;

	bool valid() const { return handle != nullptr; }

	void join()
	{
		if (!valid())
			return;
		handle->join();
		delete handle;
		handle = nullptr;
	}

	bool joinable() const
	{
		if (!valid())
			return false;
		return handle->joinable();
	}

	void detach()
	{
		if (!valid())
			return;
		handle->detach();
		delete handle;
		handle = nullptr;
	}
};

enum class threadPriority
{
	lowest,
	belowNormal,
	normal,
	aboveNormal,
	highest,
	realTime
};

namespace asyncThreadDetails
{
	inline void setThreadNameAndPriority(const threadHandle& handle, const std::wstring& threadName, threadPriority threadPriority = threadPriority::normal)
	{
		if (handle.handle == nullptr)
			return;

#ifdef _WIN32
		int priority = THREAD_PRIORITY_NORMAL;
		switch (threadPriority)
		{
		case threadPriority::lowest:		priority = THREAD_PRIORITY_LOWEST;		  break;
		case threadPriority::belowNormal:	priority = THREAD_PRIORITY_BELOW_NORMAL;  break;
		case threadPriority::aboveNormal:	priority = THREAD_PRIORITY_ABOVE_NORMAL;  break;
		case threadPriority::realTime:		priority = THREAD_PRIORITY_TIME_CRITICAL; break;
		case threadPriority::highest:		priority = THREAD_PRIORITY_HIGHEST;		  break;
		case threadPriority::normal:
		default:							priority = THREAD_PRIORITY_NORMAL;		  break;
		}
		SetThreadPriority(handle.handle->native_handle(), priority);
		if (!threadName.empty())
			SetThreadDescription(handle.handle->native_handle(), threadName.c_str());
#else
		sched_param sch;
		int policy;
		pthread_getschedparam(handle.handle->native_handle(), &policy, &sch);
		switch (threadPriority)
		{
		case threadPriority::lowest:
		case threadPriority::belowNormal:	sch.sched_priority = 10;  break;
		case threadPriority::aboveNormal:	sch.sched_priority = -5;  break;
		case threadPriority::highest:		sch.sched_priority = -10; break;
		case threadPriority::realTime:		sch.sched_priority = -20; break;
		case threadPriority::normal:
		default:							sch.sched_priority = 0;   break;
		}
		pthread_setschedparam(handle.handle->native_handle(), SCHED_FIFO, &sch);
		//if (!threadName.empty())
		//	pthread_setname_np(handle.handle->native_handle(), threadName.c_str());
#endif
	}

	template<class Fty, class... ArgTypes>
	threadHandle createAsyncThread(Fty&& Fnarg, ArgTypes&&...Args)
	{
		threadHandle handle;
		handle.handle = new std::thread(std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
		return handle;
	}
}

//create thread with priority
template<class Fty, class... ArgTypes>
threadHandle asyncThread(threadPriority priority, Fty&& Fnarg, ArgTypes&&...Args)
{
	const threadHandle handle = asyncThreadDetails::createAsyncThread(std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
	asyncThreadDetails::setThreadNameAndPriority(handle, std::wstring(), priority);
	return handle;
}

//create thread with thread name
template<class Fty, class... ArgTypes>
threadHandle asyncThread(std::string threadName, Fty&& Fnarg, ArgTypes&&...Args)
{
	const threadHandle handle = asyncThreadDetails::createAsyncThread(std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
	asyncThreadDetails::setThreadNameAndPriority(handle, getWString(threadName), threadPriority::normal);
	return handle;
}

template<class Fty, class... ArgTypes>
threadHandle asyncThread(std::wstring threadName, Fty&& Fnarg, ArgTypes&&...Args)
{
	const threadHandle handle = asyncThreadDetails::createAsyncThread(std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
	asyncThreadDetails::setThreadNameAndPriority(handle, threadName, threadPriority::normal);
	return handle;
}

//create thread with priority and thread name
template<class Fty, class... ArgTypes>
threadHandle asyncThread(threadPriority priority, std::string threadName, Fty&& Fnarg, ArgTypes&&...Args)
{
	const threadHandle handle = asyncThreadDetails::createAsyncThread(std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
	asyncThreadDetails::setThreadNameAndPriority(handle, getWString(threadName), priority);
	return handle;
}

//create thread with priority and thread name
template<class Fty, class... ArgTypes>
threadHandle asyncThread(threadPriority priority, std::wstring threadName, Fty&& Fnarg, ArgTypes&&...Args)
{
	const threadHandle handle = asyncThreadDetails::createAsyncThread(std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
	asyncThreadDetails::setThreadNameAndPriority(handle, threadName, priority);
	return handle;
}

//create thread with default priority
template<class Fty, class... ArgTypes>
threadHandle asyncThread(Fty&& Fnarg, ArgTypes&&...Args)
{
	return asyncThread(threadPriority::normal, std::forward<Fty>(Fnarg), std::forward<ArgTypes>(Args)...);
}
