#include "TimeCount.h"

CTimeCount::CTimeCount()
{
	QueryPerformanceFrequency(&m_lgFrequency);
	reset();
}

CTimeCount::~CTimeCount()
{
}

void CTimeCount::reset()
{
	QueryPerformanceCounter(&m_lgStart);
}

float CTimeCount::getCountTime() const
{
	LARGE_INTEGER lgEnd;
	QueryPerformanceCounter(&lgEnd);
	return double(lgEnd.QuadPart - m_lgStart.QuadPart) / (m_lgFrequency.QuadPart / 1000);
}