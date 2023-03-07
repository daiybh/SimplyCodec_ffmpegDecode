#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <minwindef.h>
#include <consoleapi2.h>
#include <cstdlib>

#define TEXT_COLOR_NORMAL		(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED )
#define TEXT_COLOR_WHITE		(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED|FOREGROUND_INTENSITY )
#define TEXT_COLOR_RED			(FOREGROUND_RED|FOREGROUND_INTENSITY )
#define TEXT_COLOR_GREEN		(FOREGROUND_GREEN|FOREGROUND_INTENSITY )
#define TEXT_COLOR_BLUE			(FOREGROUND_BLUE|FOREGROUND_INTENSITY )

enum struct ConsoleColors
{
	BLACK = 0,
	DARKBLUE = FOREGROUND_BLUE,
	DARKGREEN = FOREGROUND_GREEN,
	DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
	DARKRED = FOREGROUND_RED,
	DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	GRAY = FOREGROUND_INTENSITY,
	BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
};

class ConsoleUtil
{
public:
	ConsoleUtil()
	{
		CONSOLE_CURSOR_INFO info;
		info.bVisible = FALSE;
		info.dwSize = 100;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}

	~ConsoleUtil() = default;

	void gotoXY(SHORT x, SHORT y)
	{
		const COORD coord = { x, y };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void changeColour(WORD theColour)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), theColour);
	}

	void clearScreen()
	{
		system("cls");
	}
};

#else

#endif