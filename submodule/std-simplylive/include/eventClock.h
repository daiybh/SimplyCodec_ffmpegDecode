#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <fcntl.h>
#include <atomic>
#include <ctime>
#include <semaphore.h>
#endif

class EventClock
{
public:
	EventClock(bool bManualReset = false)
	{
#ifdef _WIN32
		m_handle = CreateEventA(nullptr, bManualReset ? TRUE : FALSE, FALSE, nullptr);
#else
		_sem_count = 0;
		_b_manual_reset = bManualReset;
		if (_sem_name.empty())
		{
			if (0 != sem_init(&_sem, 0, _sem_count))
			{
				_p_named_sem = SEM_FAILED;
				return;
			}
			_p_named_sem = &_sem;
		}
		else
		{
			_p_named_sem = sem_open(_sem_name.c_str(), O_CREAT, _lpEventAttributes, _sem_count);
			if (_p_named_sem == SEM_FAILED)
			{
				sem_unlink(_sem_name.c_str());
				_sem_name.clear();
				return;
			}
		}
		return;
#endif
	}

	explicit EventClock(const char* name, bool bManualReset = false)
	{
#ifdef _WIN32
		m_handle = CreateEventA(nullptr, bManualReset ? TRUE : FALSE, FALSE, name);
#else
		_sem_count = 0;
		_b_manual_reset = bManualReset;
		if (name != nullptr)
			_sem_name = name;
		if (_sem_name.empty())
		{
			if (0 != sem_init(&_sem, 0, _sem_count))
			{
				_p_named_sem = SEM_FAILED;
				return;
			}
			_p_named_sem = &_sem;
		}
		else
		{
			_p_named_sem = sem_open(_sem_name.c_str(), O_CREAT, _lpEventAttributes, _sem_count);
			if (_p_named_sem == SEM_FAILED)
			{
				sem_unlink(_sem_name.c_str());
				_sem_name.clear();
				return;
			}
		}
		return;
#endif
	}

	~EventClock()
	{
#ifdef _WIN32
		if (m_handle != nullptr)
			CloseHandle(m_handle);
#else
		if (_p_named_sem != SEM_FAILED)
		{
			if (_sem_name.empty())
			{
				sem_destroy(&_sem);
				_p_named_sem = SEM_FAILED;
			}
			else
			{
				sem_close(_p_named_sem);
				sem_unlink(_sem_name.c_str());
				_sem_name.clear();
			}
		}
#endif
	}

	void raiseEvent()
	{
#ifdef _WIN32
		if (m_handle != nullptr)
			SetEvent(m_handle);
#else
		if (_p_named_sem == nullptr)
			return;
		int32_t n_ret = sem_post(_p_named_sem);
#endif
	}

	void resetEvent()
	{
#ifdef _WIN32
		if (m_handle != nullptr)
			ResetEvent(m_handle);
#else
		if (_p_named_sem == nullptr)
			return;
		_b_initial_state = false;
		while (sem_getvalue(_p_named_sem, &_sem_count) == 0 && _sem_count > 0)
			sem_wait(_p_named_sem);
#endif
	}

	bool waitEvent(int nWaitTime = 100)
	{
#ifdef _WIN32
		if (m_handle == nullptr)
			return false;
		const int ret = WaitForSingleObject(m_handle, nWaitTime);
		return ret == WAIT_OBJECT_0;
#else
		if (_p_named_sem == nullptr)
			return false;

		if (_b_initial_state)
		{
			if (!_b_manual_reset)
			{
				_b_initial_state = false;
				while (sem_getvalue(_p_named_sem, &_sem_count) == 0 && _sem_count > 0)
					sem_wait(_p_named_sem);
			}
			return true;
		}
		if (nWaitTime == 0)
		{
			const int32_t n_ret = sem_wait(_p_named_sem);
			if (n_ret != 0)
				return false;
		}
		else
		{
			int32_t  n_ret = 0;
			timespec ts = sem_get_time_millsecs(nWaitTime);
			while ((n_ret = sem_timedwait(_p_named_sem, &ts)) == -1 && errno == EINTR)
			{
			}
			if (n_ret == -1)
			{
				if (errno == ETIMEDOUT)
					return false;
				return false;
			}
		}
		if (_b_manual_reset)
			_b_initial_state = true;
		return true;
#endif
	}

private:
#ifdef _WIN32
	void* m_handle = nullptr;
#else
	bool             _b_manual_reset = false;
	std::atomic_bool _b_initial_state = false;
	sem_t*           _p_named_sem = SEM_FAILED;
	sem_t            _sem;
	std::string      _sem_name;
	long             _lpEventAttributes = 0777;
	int32_t          _sem_count = 0;

	timespec sem_get_time_millsecs(long msecs)
	{
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		long secs = msecs / 1000;
		msecs = msecs % 1000;

		long add = 0;
		msecs = msecs * 1000 * 1000 + ts.tv_nsec;
		add = msecs / (1000 * 1000 * 1000);
		ts.tv_sec += (add + secs);
		ts.tv_nsec = msecs % (1000 * 1000 * 1000);
		return ts;
	}
#endif
};
