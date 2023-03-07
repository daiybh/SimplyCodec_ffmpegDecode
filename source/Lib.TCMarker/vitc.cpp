#include "vitc.h"
#include <Windows.h>

//-----------------------------------------------------------------------------

CVITC::CVITC()
{
	m_real = false;
	m_hours = 0;
	m_minutes = 0;
	m_seconds = 0;
	m_frames = 0;
	m_format = 25;
}

//-----------------------------------------------------------------------------

void CVITC::setFormat(FPTVideoFormat _format)
{
	switch (_format)
	{
	case FP_FORMAT_1080i_5000: m_format = 25; break;
	case FP_FORMAT_1080i_5994: m_format = 29; break;
	case FP_FORMAT_1080i_6000: m_format = 30; break;
	case FP_FORMAT_1080p_2400: m_format = 24; break;//
	case FP_FORMAT_1080p_2500: m_format = 25; break;
	case FP_FORMAT_1080p_2997: m_format = 29; break;
	case FP_FORMAT_1080p_3000: m_format = 30; break;
	case FP_FORMAT_1080p_5000: m_format = 50; break;
	case FP_FORMAT_1080p_5994: m_format = 59; break;
	case FP_FORMAT_1080p_6000: m_format = 60; break;
	case FP_FORMAT_720p_2500:  m_format = 25; break;
	case FP_FORMAT_720p_2997:  m_format = 29; break;
	case FP_FORMAT_720p_5000:  m_format = 50; break;
	case FP_FORMAT_720p_5994:  m_format = 59; break;
	case FP_FORMAT_720p_6000:  m_format = 60; break;
	case FP_FORMAT_4Kp_2400:   m_format = 24; break;
	case FP_FORMAT_4Kp_2500:   m_format = 25; break;
	case FP_FORMAT_4Kp_2997:   m_format = 29; break;
	case FP_FORMAT_4Kp_3000:   m_format = 30; break;
	case FP_FORMAT_4Kp_5000:   m_format = 50; break;
	case FP_FORMAT_4Kp_5994:   m_format = 59; break;
	case FP_FORMAT_4Kp_6000:   m_format = 60; break;
	case FP_FORMAT_SD_525_5994:m_format = 29; break;
	case FP_FORMAT_SD_625_5000:m_format = 25; break;
	case FP_FORMAT_8kp_2400: m_format = 24; break;
	case FP_FORMAT_8kp_2500: m_format = 25; break;
	case FP_FORMAT_8kp_2997: m_format = 29; break;
	case FP_FORMAT_8kp_3000: m_format = 30; break;
	case FP_FORMAT_8kp_5000: m_format = 50; break;
	case FP_FORMAT_8kp_5994: m_format = 59; break;
	case FP_FORMAT_8kp_6000: m_format = 60; break;

		case FP_FORMAT_UNKNOWN:
	default: m_format = 25; break;
	
	}
}

//-----------------------------------------------------------------------------

unsigned char CVITC::getFormat()
{
	return m_format;
}

void CVITC::inc()
{
	unsigned char maxFrame = 25;
	switch (m_format)
	{
	case 29:
	case 30: maxFrame = 30; break;
	case 50:maxFrame = 50; break;
	case 59:maxFrame = 60; break;
	}
	m_real = false;
	if (++m_frames == maxFrame)
	{
		if ((m_format == 29) && (m_seconds == 59) && ((m_minutes % 10) != 9))
			m_frames = 2;
		else
			m_frames = 0;
		if (++m_seconds == 60)
		{
			m_seconds = 0;
			if (++m_minutes == 60)
			{
				m_minutes = 0;
				if (++m_hours == 24)
					m_hours = 0;
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CVITC::set(bool real, int _frameFromMidNight)
{
	m_real = real;

	unsigned char framePerSec = m_format;

	if (m_format == 23)
	{
		/*int nb10min = _frameFromMidNight / 17982;
		int withIn10min = _frameFromMidNight % 17982;
		if (withIn10min >= 1440)
			_frameFromMidNight += ((withIn10min - 2) / 1438) * 2;
		_frameFromMidNight += (nb10min * 18);*/
		framePerSec = 24;
	}
	else if ((m_format == 29) || (m_format == 59))
	{
		int nb10min = _frameFromMidNight / 17982;
		int withIn10min = _frameFromMidNight % 17982;
		if (withIn10min >= 1800)
			_frameFromMidNight += ((withIn10min - 2) / 1798) * 2;
		_frameFromMidNight += (nb10min * 18);
		framePerSec = 30;
	}
	else if (m_format == 50)
		framePerSec = 25;
	else if (m_format == 60)
		framePerSec = 30;

	m_frames = _frameFromMidNight % framePerSec;
	_frameFromMidNight /= framePerSec;
	m_seconds = _frameFromMidNight % 60;
	_frameFromMidNight /= 60;
	m_minutes = _frameFromMidNight % 60;
	m_hours = _frameFromMidNight / 60;
}

void CVITC::set(bool real, unsigned char hours, unsigned char minutes, unsigned char seconds, unsigned char frames)
{
	m_real = real;
	m_hours = hours;
	m_minutes = minutes;
	m_seconds = seconds;
	m_frames = frames;
}
/*
void CVITC::set(LONGLONG ref)
{
	m_real = false;
}
*/
//-----------------------------------------------------------------------------

void CVITC::add(unsigned long frameNum, unsigned char h, unsigned char m, unsigned char s, unsigned char f)
{
	unsigned char maxFrame = (m_format == 25 || m_format == 50) ? 25 : 30;
	m_real = false;

	m_frames = f;
	m_seconds = s;
	m_minutes = m;
	m_hours = h;

	for (int i = 0; i < frameNum; i++)
	{
		if (++m_frames == maxFrame)
		{
			if ((m_format == 29) && (m_seconds == 59) && ((m_minutes % 10) != 9)) // each min decrease 2 frames except per 10 mins
				m_frames = 2;
			else
				m_frames = 0;
			if (++m_seconds == 60)
			{
				m_seconds = 0;
				if (++m_minutes == 60)
				{
					m_minutes = 0;
					if (++m_hours == 24)
						m_hours = 0;
				}
			}
		}
	}
}
void CVITC::loadFromPCtime(unsigned char& h, unsigned char& m, unsigned char& s, unsigned char& f)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	m_real = false;
	//	m_originalHours = (unsigned char)st.wHour;
	//	m_originalMinutes = (unsigned char)st.wMinute;
	//	m_orginalSeconds = (unsigned char)st.wSecond;

	h = (unsigned char)st.wHour;
	m = (unsigned char)st.wMinute;
	s = (unsigned char)st.wSecond;
	int msperframe = 40;
	switch (m_format)
	{
	case 25: msperframe = 1000 / 25; break;
	case 30: msperframe = 1000 / 30; break;
	case 29: msperframe = 1001 / 29; break;
	default:
		break;
	}

	//	m_orginalFrames = st.wMilliseconds/msperframe;
	f = st.wMilliseconds / msperframe;
}

//-----------------------------------------------------------------------------