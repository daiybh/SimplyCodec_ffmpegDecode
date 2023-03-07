#include "easyThread.h"
#include <windows.h>
#include <cstdio>

EasyThread::EasyThread()
{
	m_thread = nullptr;
	m_exitRequested = false;
}

EasyThread::~EasyThread()
{
}

void EasyThread::startThread(Priority _priority)
{
	m_exitRequested = false;
	m_thread = CreateThread(nullptr, 0, theThread, (void*)this, CREATE_SUSPENDED, &m_threadId);
	switch (_priority)
	{
	case above_normal:	SetThreadPriority(m_thread, THREAD_PRIORITY_ABOVE_NORMAL); break;
	case real_time:		SetThreadPriority(m_thread, THREAD_PRIORITY_TIME_CRITICAL); break;
	case normal:
	default:			SetThreadPriority(m_thread, THREAD_PRIORITY_NORMAL); break;
	}
	ResumeThread(m_thread);
}

void EasyThread::stopThread(unsigned long _timeOut)
{
	m_exitRequested = true;
	if (m_thread)
	{
		WaitForMultipleObjects(1, &m_thread, TRUE, _timeOut);
		CloseHandle(m_thread);
		m_thread = nullptr;
	}
}

bool EasyThread::isRunning() const
{
	return !m_exitRequested;
}

unsigned long __stdcall EasyThread::theThread(void* lpParam)
{
	EasyThread* pEasyThread = (EasyThread *)lpParam;
	pEasyThread->callBack();
	return 0;
}