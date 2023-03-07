#pragma once

class EventClock
{
	void* m_event = nullptr;
public:
	EventClock();
	explicit EventClock(char* _evtName);
	~EventClock();

	void raiseEvent() const;
	bool waitEvent(int nWaitTime = 50) const;
};
