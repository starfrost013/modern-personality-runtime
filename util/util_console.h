#pragma once

//
//	UtilConsole.h: Implements console utilities. Uses ANSI command codes (Windows 10 v1511+ / Linux)
//

typedef enum ConsoleColor_s
{
	ConsoleColor_Black = 0,
	ConsoleColor_Red = 1,
	ConsoleColor_Green = 2,
	ConsoleColor_Yellow = 3,
	ConsoleColor_Blue = 4,
	ConsoleColor_Magenta = 5,
	ConsoleColor_Cyan = 6,
	ConsoleColor_White = 7,
	// this is just used to indicate the first "bright" console colour so other code is easier to read
	CONSOLECOLOR_FIRST_BRIGHT = 8,
	ConsoleColor_BrightBlack = 8,
	ConsoleColor_BrightRed = 9,
	ConsoleColor_BrightGreen = 10,
	ConsoleColor_BrightYellow = 11,
	ConsoleColor_BrightBlue = 12,
	ConsoleColor_BrightMagenta = 13,
	ConsoleColor_BrightCyan = 14,
	ConsoleColor_BrightWhite = 15,
} ConsoleColor;

void Util_ConsoleSetForegroundColor(ConsoleColor color);
void Util_ConsoleSetBackgroundColor(ConsoleColor color);
void Util_ConsoleResetForegroundColor();
void Util_ConsoleResetBackgroundColor();
void Util_ConsoleClearScreen();