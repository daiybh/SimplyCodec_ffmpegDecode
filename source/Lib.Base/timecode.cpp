#include "timecode.h"

Timecode::Timecode()
{
	reset();
}

Timecode::Timecode(char _hour, char _minute, char _second, char _frame)
{
	hour = _hour;
	minute = _minute;
	second = _second;
	frame = _frame;
}

void Timecode::inc(char _maxFrame)
{
	if (++frame == _maxFrame)
	{
		frame = 0;
		if (++second == 60)
		{
			second = 0;
			if (++minute == 60)
			{
				minute = 0;
				if (++hour == 24)
					hour = 0;
			}
			else if ((_maxFrame == 30 || _maxFrame == 60) && (minute % 10))
				frame = _maxFrame == 30 ? 2 : 4;
		}
	}
}

void Timecode::decrease(char _maxFrame, int _nOffset)
{
	for (int i = 0; i < _nOffset; i++)
		decrease(_maxFrame);
}

void Timecode::decrease(char _maxFrame)
{
	if ((((_maxFrame == 30 && frame == 2) || (_maxFrame == 60 && frame == 4)) && minute % 10) || frame == 0)
	{
		frame = _maxFrame - 1;
		if (second-- == 0)
		{
			second = 59;
			if (minute-- == 0)
			{
				minute = 59;
				if (hour-- == 0)
					hour = 23;
			}
		}
	}
	else
		--frame;
}

bool Timecode::compare(const Timecode& _tc) const
{
	return hour == _tc.hour && minute == _tc.minute && second == _tc.second && frame == _tc.frame;
}

void Timecode::setTC(char _hour, char _minute, char _second, char _frame)
{
	hour = _hour;
	minute = _minute;
	second = _second;
	frame = _frame;
}

void Timecode::setTC(const Timecode& _tc)
{
	hour = _tc.hour;
	minute = _tc.minute;
	second = _tc.second;
	frame = _tc.frame;
}

string Timecode::getTCString(char _maxFrame) const
{
	char tc[20] = { 0 };
	if (_maxFrame != 30 && _maxFrame != 60)
		sprintf_s(tc, "%2.2d:%2.2d:%2.2d:%2.2d ", hour, minute, second, frame);
	else
	{
		if (_maxFrame == 30)
			sprintf_s(tc, "%2.2d:%2.2d:%2.2d;%2.2d ", hour, minute, second, frame);
		else if (_maxFrame == 60)
			sprintf_s(tc, "%2.2d:%2.2d:%2.2d;%2.2d%s", hour, minute, second, frame / 2, (frame % 2 == 0) ? " " : ".");
	}

	return tc;
}

Timecode Timecode::make_tc(char _hour, char _minute, char _second, char _frame)
{
	Timecode tc(_hour, _minute, _second, _frame);
	return tc;
}

void Timecode::reset()
{
	memset(this, 0, sizeof(Timecode));
}

bool Timecode::operator!=(const Timecode& _tc) const
{
	return hour != _tc.hour || minute != _tc.minute || second != _tc.second || frame != _tc.frame;
}

bool Timecode::operator==(const Timecode& _tc) const
{
	return hour == _tc.hour && minute == _tc.minute && second == _tc.second && frame == _tc.frame;
}