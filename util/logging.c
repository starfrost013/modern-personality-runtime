#include "logging.h"
#include "util/util.h"
#include "util/util_console.h"
#include "macros.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototypes for "internal" functions only used in this source file
void	Logging_Log(const char* text, LogChannel channel, va_list args);

Logger* sys_logger;

// Implementation
Logger* Logger_new()
{
	Logger* logger = (Logger*)calloc(1, sizeof(Logger));

	assert(logger != NULL);
	if (logger == NULL) return NULL;		//Shutup MSVC warnings 

	//todo: allow this to be configured in settings
	logger->settings = LogSettings_new(u8"latest.log", LogChannel_Debug | LogChannel_Message | LogChannel_Warning | LogChannel_Error | LogChannel_Fatal, 
		LogSource_Printf | LogSource_File, false);
	logger->initialised = true;

	if (Util_EnumHasFlag(logger->settings->source, LogSource_File))
	{
		logger->handle = fopen(logger->settings->fileName, "w+");

		if (logger->handle == NULL)
		{
			printf(u8"Log failed: 0x0002DEAD Error opening logfile %s: errno %d\n", logger->settings->fileName, errno);
			return NULL;
		}
	}

	return logger;
}

void Logger_destroy(Logger* logger)
{
	fclose(logger->handle);
	free(logger->handle);
	LogSettings_destroy(logger->settings);
	free(logger);
}

LogSettings* LogSettings_new(const char* fileName, LogChannel channels, LogSource source, bool keepOldLogs)
{
	assert(fileName != NULL);

	LogSettings* settings = (LogSettings*)calloc(1, sizeof(LogSettings));

	assert(settings != NULL);
	if (settings == NULL) return NULL;		//Shutup MSVC warnings 

	settings->fileName = fileName;
	settings->channels = channels;
	settings->source = source;
	settings->keepOldLogs = keepOldLogs;
	return settings;
}

void LogSettings_destroy(LogSettings* settings)
{
	free(settings);
}

bool Logging_Init()
{
	sys_logger = Logger_new();

	return (sys_logger != NULL);
}

void Logging_LogChannel(const char* text, LogChannel channel, ...)
{
	va_list	args;

	va_start(args, channel);

	Logging_Log(text, channel, args);
}

void Logging_LogAll(const char* text, ...)
{
	va_list args;

	va_start(args, text);

	Logging_Log(text, LogChannel_Debug | LogChannel_Message | LogChannel_Warning | LogChannel_Error | LogChannel_Fatal, args);
}

// yes this is required
void Logging_Log(const char* text, LogChannel channel, va_list args)
{
	if (!Util_EnumHasFlag(sys_logger->settings->channels, channel))
	{
		printf(u8"Log failed: 0x0003DEAD Tried to output to a closed log channel (%d). Refer to logging.h!\n", channel);
		return;
	}

// if solely printing debug channel, return on release build
#if !X86_DEBUG
	if (channel == LogChannel_Debug)
	{
		return;
	}
#endif

	char date_buffer[LOGGING_MAX_LENGTH_DATE] = {0};
	char log_string_buffer[LOGGING_MAX_LENGTH_TOTAL] = {0};

	Util_DateGetCurrentString(&date_buffer);

	int date_buffer_length = strlen(date_buffer);

	// avoids a crash by trying to decrement 1 from this value...whoops
	if (date_buffer_length == 0)
	{
		return;
	}

	if (strlen(text) > LOGGING_MAX_LENGTH_TEXT)
	{
		printf(u8"Log failed: 0x0004DEAD cannot log string of length 0 or above %d bytes!", LOGGING_MAX_LENGTH_TEXT);
		return;
	}

	// lop off the last character so it doesn't have a new line

	date_buffer[date_buffer_length - 1] = '\0';

	const char* prefix = "[";
	const char* date_suffix = "]: ";

	// print separate colours
	switch (channel)
	{
		case LogChannel_Debug:
			prefix = "[DEBUG] [";
			break;		
		case LogChannel_Warning:
			prefix = "[WARNING] [";
			break;
		case LogChannel_Error:
			prefix = "[ERROR] [";
			break;
		case LogChannel_Fatal:
			prefix = "[FATAL] [";
			break;
	}

	const char* suffix = " \n";

	assert(date_buffer[0] != NULL);

	strncat(log_string_buffer, prefix, sizeof(log_string_buffer));
	strncat(log_string_buffer, date_buffer, sizeof(log_string_buffer));
	strncat(log_string_buffer, date_suffix, sizeof(log_string_buffer));
	strncat(log_string_buffer, text, sizeof(log_string_buffer));
	strncat(log_string_buffer, suffix, sizeof(log_string_buffer));

	if (Util_EnumHasFlag(sys_logger->settings->source, LogSource_Printf))
	{
		switch (channel)
		{
			case LogChannel_Debug:
				Util_ConsoleSetForegroundColor(ConsoleColor_Blue);
				break;
			case LogChannel_Warning:
				Util_ConsoleSetForegroundColor(ConsoleColor_Yellow);
				break;
			case LogChannel_Error:
			case LogChannel_Fatal:
				Util_ConsoleSetForegroundColor(ConsoleColor_Red);
				break;
		}

		vprintf(log_string_buffer, args);

		Util_ConsoleResetForegroundColor();
	}

	if (Util_EnumHasFlag(sys_logger->settings->source, LogSource_File))
	{
		vfprintf(sys_logger->handle, log_string_buffer, args);
	}

	va_end(args);
}

void Logging_Shutdown()
{
	Logger_destroy(sys_logger);
}