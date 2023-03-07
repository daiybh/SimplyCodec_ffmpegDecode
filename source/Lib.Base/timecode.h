#pragma once
#include <string>
using namespace std;

struct Timecode
{
	char hour;
	char minute;
	char second;
	char frame;

	Timecode();
	Timecode(char _hour, char _minute, char _second, char _frame);

	void inc(char _maxFrame);
	void decrease(char _maxFrame, int _nOffset);
	void decrease(char _maxFrame);
	bool compare(const Timecode& _tc) const;
	bool operator==(const Timecode& _tc) const;
	bool operator!=(const Timecode& _tc) const;
	void setTC(char _hour, char _minute, char _second, char _frame);
	void setTC(const Timecode& _tc);
	string getTCString(char _maxFrame) const;
	static Timecode make_tc(char _hour, char _minute, char _second, char _frame);
	void reset();
};
