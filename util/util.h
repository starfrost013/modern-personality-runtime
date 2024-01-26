#pragma once
#include "../core.h"

//
//	Util.h: Implements "general" util functions.
//

// Date utilities
void Util_DateGetCurrentString(char *dateStr);								// Gets the current date and returns it as a C time format string.

// Enum utilities
bool Util_EnumHasFlag(int32_t enumValue, int32_t flag);						// Checks if an enum value has a specific flag set set.

// String utilities
char* Util_StringFromInt(int32_t num);										// Converts a string from an integer.
bool Util_StringFromBoolean(char* string);									// Converts a boolean from a string.
char* Util_StringLTrim(char* string);										// Left-trims a string (all whitespace characters on the left side of the string are removed)
char* Util_StringRTrim(char* string);										// Right-trims a string (all whitespace characters on the right side of the string are removed)
char* Util_StringTrim(char* string);										// Trims a string (all whitespace characters on both sides of the stirng are removed)