#pragma once
#include "../Lib.Base/videoFormat.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

class CVITC
{
	bool		  m_real;
	unsigned char m_hours;
	unsigned char m_minutes;
	unsigned char m_seconds;
	unsigned char m_frames;
	unsigned char m_format;
public:
	CVITC();
	void inc();
	void loadFromPCtime(unsigned char& h, unsigned char& m, unsigned char& s, unsigned char& f);

	void setFormat(FPTVideoFormat _format);
	unsigned char getFormat();
	void set(bool real, int _frameFromMidNight);
	void set(bool real, unsigned char hours, unsigned char minutes, unsigned char seconds, unsigned char frames);
	void add(unsigned long frameNum, unsigned char h, unsigned char m, unsigned char s, unsigned char f);

	bool isReal() { return m_real; }
	unsigned char getHours() { return m_hours; }
	unsigned char getMinutes() { return m_minutes; }
	unsigned char getSeconds() { return m_seconds; }
	unsigned char getFrames() { return m_frames; }
	char time[255];
	char *getString()
	{		
		sprintf_s(time, "%02d:%02d:%02d:%02d", getHours(), getMinutes(), getSeconds(), getFrames());
		return time;
	}
};

//-----------------------------------------------------------------------------
