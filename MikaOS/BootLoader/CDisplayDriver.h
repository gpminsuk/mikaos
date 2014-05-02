#pragma once

#include "Global.h"

#define	SCREEN_COLOR	0x1F
#define	SCREEN_WIDTH	80

class CDisplayDriver
{
public:
	CDisplayDriver();
	~CDisplayDriver();

	unsigned short CursorPosX;
	unsigned short CursorPosY;

	void ClearScreen();
	void PrintCharacter(unsigned char c);
	void SetCursor(unsigned short col, unsigned short row);
	void Newline();
	void UpdateCursor();

	static void PrintString(const char* format, ...);
	static void Format(const char* format, va_list args);
};

