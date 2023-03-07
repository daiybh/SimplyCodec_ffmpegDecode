#pragma once
#include <cstdint>
#include "clockTimer.h"

class FpsHelper
{
public:

	int32_t update()
	{
		if (m_timer.elapse_ms() >= 1000.0)
		{
			m_timer.reset();
			m_fps = m_counter;
			m_counter = 0;
		}
		++m_counter;
		return m_fps;
	}

	int32_t getFps() const
	{
		return m_fps;
	}

	void reset()
	{
		m_timer.reset();
		m_counter = 0;
		m_fps = 0;
	}

private:
	ClockTimer m_timer;
	int32_t    m_counter = 0;
	int32_t    m_fps = 0;
};
