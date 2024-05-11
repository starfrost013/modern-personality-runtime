#include "util_console.h"
#include "util/util.h"

#include <stdio.h>
#include <string.h>

// various buffer size demands
#define CONSOLE_TERMINAL_COMMAND_PREFIX "\x1B["
#define CONSOLE_COLOR_BUFFER_SIZE		17

void Util_ConsoleSetForegroundColor(ConsoleColor color)
{
	int32_t finalColor = 0;

	finalColor = 30 + color;
	if (color >= CONSOLECOLOR_FIRST_BRIGHT) finalColor = 90 + color;

	// 10 (max) + 1 + 4 + 1 for safety
	char final_string[CONSOLE_COLOR_BUFFER_SIZE] = {0};

	char* temp_string = Util_StringFromInt(finalColor);

	int remaining_characters = CONSOLE_COLOR_BUFFER_SIZE - 4;

	strncat(&final_string, CONSOLE_TERMINAL_COMMAND_PREFIX, remaining_characters);
	remaining_characters -= strlen(temp_string);
	strncat(&final_string, temp_string, remaining_characters);
	remaining_characters -= 1;
	strncat(&final_string, "m", remaining_characters);

	printf(final_string);
}

void Util_ConsoleSetBackgroundColor(ConsoleColor color)
{
	int32_t final_color = 0;

	final_color = 40 + color;
	if (color >= CONSOLECOLOR_FIRST_BRIGHT) final_color = 100 + color;

	// 10 (max) + 1 + 4 + 1 for safety
	char* final_string[CONSOLE_COLOR_BUFFER_SIZE] = {0};

	char* temp_string = Util_StringFromInt(final_color);

	int remaining_characters = CONSOLE_COLOR_BUFFER_SIZE - 4;

	strncat(final_string, CONSOLE_TERMINAL_COMMAND_PREFIX, remaining_characters);
	remaining_characters -= strlen(temp_string);
	strncat(final_string, temp_string, remaining_characters);
	remaining_characters -= 1;
	strncat(final_string, "m", remaining_characters);

	printf(final_string);
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