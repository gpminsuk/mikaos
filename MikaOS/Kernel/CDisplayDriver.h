#pragma once

typedef __builtin_va_list		va_list;
#define va_start(v,l)	__builtin_va_start(v,l)
#define va_arg(v,l)		__builtin_va_arg(v,l)
#define va_end(v)		__builtin_va_end(v)
#define va_copy(d,s)	__builtin_va_copy(d,s)

extern "C" unsigned char AsmIn(unsigned long long Port);
extern "C" void AsmOut(unsigned long long Port, unsigned char Value);

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

extern CDisplayDriver GDisplayDriver;