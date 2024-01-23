#pragma once
#include "../core.h"
#include "util_console.h"

// various buffer size demands
#define CONSOLE_TERMINAL_COMMAND_PREFIX "\x1B["
#define CONSOLE_COLOR_BUFFER_SIZE		17

void Util_ConsoleSetForegroundColor(ConsoleColor color)
{
	int32_t finalColor = 0;

	finalColor = 30 + color;
	if (color >= CONSOLECOLOR_FIRST_BRIGHT) finalColor = 90 + color;

	// 10 (max) + 1 + 4 + 1 for safety
	char finalString[CONSOLE_COLOR_BUFFER_SIZE];
	memset(finalString, 0x00, sizeof(char) * CONSOLE_COLOR_BUFFER_SIZE);

	char* string = Util_StringFromInt(finalColor);

	int remaining_characters = CONSOLE_COLOR_BUFFER_SIZE - 4;

	strncat(&finalString, CONSOLE_TERMINAL_COMMAND_PREFIX, remaining_characters);
	remaining_characters -= strlen(string);
	strncat(&finalString, string, remaining_characters);
	remaining_characters -= 1;
	strncat(&finalString, "m", remaining_characters);

	printf(finalString);
}

void Util_ConsoleSetBackgroundColor(ConsoleColor color)
{
	int32_t finalColor = 0;

	finalColor = 40 + color;
	if (color >= CONSOLECOLOR_FIRST_BRIGHT) finalColor = 100 + color;

	// 10 (max) + 1 + 4 + 1 for safety
	char* finalString[CONSOLE_COLOR_BUFFER_SIZE];
	memset(finalString, 0x00, sizeof(char) * 17);

	char* string = Util_StringFromInt(finalColor);

	int remaining_characters = CONSOLE_COLOR_BUFFER_SIZE - 4;

	strncat(finalString, CONSOLE_TERMINAL_COMMAND_PREFIX, remaining_characters);
	remaining_characters -= strlen(string);
	strncat(finalString, string, remaining_characters);
	remaining_characters -= 1;
	strncat(finalString, "m", remaining_characters);

	printf(finalString);
}

void Util_ConsoleResetForegroundColor()
{
	printf(CONSOLE_TERMINAL_COMMAND_PREFIX "39m");
}

void Util_ConsoleResetBackgroundColor()
{
	printf(CONSOLE_TERMINAL_COMMAND_PREFIX "49m");
}

void Util_ConsoleClearScreen()
{
	printf(CONSOLE_TERMINAL_COMMAND_PREFIX "2J"); // Clear screen.
	printf(CONSOLE_TERMINAL_COMMAND_PREFIX "3J"); // Clear scrollback.
	printf(CONSOLE_TERMINAL_COMMAND_PREFIX "0;0H"); // Move to 0,0.
}