#pragma once

class EasyThread
{
	void*		  m_thread;
	unsigned long m_threadId = 0;
	volatile bool m_exitRequested;

	static unsigned long __stdcall theThread(void* lpParam);

protected:
	virtual void callBack() = 0;

	enum Priority
	{
		normal,
		above_normal,
		real_time
	};

	void startThread(Priority _priority = normal);
	void stopThread(unsigned long _timeOut = 0xFFFFFFFF);
	bool isRunning() const;

public:
	EasyThread();
	virtual ~EasyThread();
};
