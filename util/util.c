#include "../core.h"
#include "util.h"

char g_string_from_intptr[10];

void Util_DateGetCurrentString(char *str)
{
	time_t current_time = 0;
    // get the local time
	time(&current_time);
	const struct tm* current_time_info_ptr = localtime(&current_time); // todo: use localtime

	assert(str != NULL);

	char* final_text = asctime(current_time_info_ptr);

	assert(final_text != NULL);

	if (final_text == NULL) return; // shut up the compiler again

	strcpy(str, final_text);
}

bool Util_EnumHasFlag(int32_t enum_value, int32_t flag)
{
	return ((enum_value & flag) == flag);
}

bool Util_StringFromBoolean(char* str)
{
	assert(str != NULL);

	if (strncmp(str, "true", 4))
	{
		return true;
	}
	else if (strncmp(str, "false", 5))
	{
		return false;
	}

	// everything else is false
	return false;
}

char* Util_StringFromInt(int32_t num)
{
	// 10 digit number as uint32_t max is 2147483647
	memset(&g_string_from_intptr, 0x00, sizeof(char) * 10);
	snprintf(&g_string_from_intptr, sizeof(char) * 10, "%d", num);
	
	return &g_string_from_intptr;
}

// Left-trims a string (all whitespace characters on the left side of the string are removed)
char* Util_StringLTrim(char* string)
{
	// check the string is not a null ptr
	if (string == NULL)	return NULL;

	size_t length = strlen(string);

	// if the string is zero length, just return the string
	if (length == 0) return string;

	// remove whitespace chars by incrementing the pointer
	for (size_t chNum = 0; chNum < length; chNum++)
	{
		if (isspace(string[chNum]))
		{
			string++;
		}
		else
		{
			break;
		}
	}

	return string;
}

// Right-trims a string (all whitespace characters on the right side of the string are removed)
char* Util_StringRTrim(char* string)
{
	// check the string is not a null ptr
	if (string == NULL)	return NULL;

	size_t length = strlen(string);

	// if the string is zero length, or just one character just return the string
	if (length <= 1) return string;

	// remove whitespace chars by incrementing the pointer
	for (size_t chNum = length - 1; chNum >= 0; chNum--)
	{
		if (isspace(string[chNum]))
		{
			string--;
		}
		else
		{
			// prevent mangling strings in the case we have strings separated from each other (like a string of the type "hello world 2"
			break;
		}
	}

	return string;
}

char* Util_StringTrim(char* string)
{
	return Util_StringLTrim(Util_StringRTrim(string));
}