#include "CDisplayDriver.h"

CDisplayDriver GDisplayDriver;

CDisplayDriver::CDisplayDriver()
{
	ClearScreen();
}

CDisplayDriver::~CDisplayDriver()
{
}

void CDisplayDriver::ClearScreen()
{
	SetCursor(0, 0);
	unsigned long long *monitor = (unsigned long long*)0xB8000;
	for (int i = 0; i < 500; ++i)
	{
		monitor[i] = SCREEN_COLOR;
		monitor[i] <<= 8;
		monitor[i] += 0x20;
		monitor[i] <<= 8;
		monitor[i] += SCREEN_COLOR;
		monitor[i] <<= 8;
		monitor[i] += 0x20;
		monitor[i] <<= 8;
		monitor[i] += SCREEN_COLOR;
		monitor[i] <<= 8;
		monitor[i] += 0x20;
		monitor[i] <<= 8;
		monitor[i] += SCREEN_COLOR;
		monitor[i] <<= 8;
		monitor[i] += 0x20;
	}
}

void CDisplayDriver::PrintCharacter(unsigned char c)
{
	unsigned short* monitor = (unsigned short*)0xB8000;
	monitor[CursorPosY*SCREEN_WIDTH + CursorPosX] = (SCREEN_COLOR << 8) + c;
	++CursorPosX;
	if (CursorPosX >= SCREEN_WIDTH)
	{
		CursorPosX = 0;
		++CursorPosY;
	}
}

void CDisplayDriver::SetCursor(unsigned short col, unsigned short row)
{
	unsigned short position = (row * SCREEN_WIDTH) + col;
	AsmOut(0x3D4, 0x0F);
	AsmOut(0x3D5, position & 0xFF);
	AsmOut(0x3D4, 0x0E);
	AsmOut(0x3D5, (position >> 8) & 0xFF);	

	CursorPosX = col;
	CursorPosY = row;
}

void CDisplayDriver::Newline()
{
	CursorPosX = 0;
	++CursorPosY;
	if (CursorPosY > 20)
	{
		ClearScreen();
		CursorPosY = 0;
	}
}

void CDisplayDriver::UpdateCursor()
{
	SetCursor(CursorPosX, CursorPosY);
}

void CDisplayDriver::PrintString(const char* format, ...)
{
	va_list arg;

	va_start(arg, format);
	Format(format, arg);
	va_end(arg);

	GDisplayDriver.Newline();
	GDisplayDriver.UpdateCursor();
}

#define SIGN	1		/* unsigned/signed long */
#define LARGE	2		/* converts lower alphabet to higher one */

void CDisplayDriver::Format(const char* format, va_list args)
{
	char temp, numtemp[33], *numptr, prefix;
	unsigned int number, base, flag, width;

	numtemp[32] = 0;

	for (; *format; ++format) {
		/* normal character */
		if (*format != '%') {
			GDisplayDriver.PrintCharacter(*format);
			continue;
		}
		++format;
		/* flag process */
		flag = 0;
		prefix = ' ';
		width = 0;

		if (*format == '0')
			prefix = '0';

		while (*format >= '0' && *format <= '9') {
			width = (width * 10) + (*(format++) - '0');
		}

		switch (*format) {
		case 'c':
			GDisplayDriver.PrintCharacter(va_arg(args, int));
			continue;
		case 's':
			numptr = va_arg(args, char*);
			while (*numptr != 0)
				GDisplayDriver.PrintCharacter(*numptr++);
			continue;
		case '%':
			GDisplayDriver.PrintCharacter('%');
			continue;
		case 'b':
			base = 2;
			break;
		case 'd':	case 'i':
			flag |= SIGN;
		case 'u':
			base = 10;
			break;
		case 'o':
			base = 8;
			break;
		case 'X':
			flag |= LARGE;
		case 'x':
			base = 16;
			break;
		default:
			continue;
		}

		/* number */
		numptr = &numtemp[31];
		number = va_arg(args, unsigned int);

		if ((flag&SIGN) && (number & 0x80000000)) {
			number = ~number + 1;	/* 2's complement */
			GDisplayDriver.PrintCharacter('-');	/* '-';;;; */
		}

		do {
			temp = number%base;
			if (temp > 9) {
				temp += ((flag & LARGE ? 'A' : 'a') - 10);
			}
			else {
				temp += '0';
			}
			*numptr-- = temp;
			if (width > 0) width--;
		} while (number /= base);

		if (width > 0) {
			while (width-- != 0)
				*numptr-- = prefix;
		}
		while ((temp = *(++numptr)) != 0) {
			GDisplayDriver.PrintCharacter(temp);
		}
	}
}