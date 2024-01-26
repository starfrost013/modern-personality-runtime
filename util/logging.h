#pragma once
#include "../core.h"
//
// Logging system
// 
// Oct 2023: Wrote it
// Jan 2024: Fixed it...and added LogChannel_Debug.
//

// Enumerates logging channels. This is a set of flags, so the user can enable anything they want.
typedef enum LogChannel_s
{
	// Prints only on debug builds.
	LogChannel_Debug = 1,

	// Prints advisory messages.
	LogChannel_Message = 2,

	// Prints warnings. The user needs to know about them, but they don't impair program operation.
	LogChannel_Warning = 4,

	// Prints errors that impair program operation.
	LogChannel_Error = 8,

	// Prints fatal errors that require the program to exit.
	LogChannel_Fatal = 16,
} LogChannel;

// Enumerates available log output sources.
typedef enum LogSource_s
{
	// Logs to standardout using vprintf.
	LogSource_Printf = 1,

	// Logs to a file. (currently "latest.log")
	LogSource_File = 2,
} LogSource;

// Struct storing log settings.
typedef struct LogSettings_s
{
	const char* fileName;
	LogChannel channels;
	LogSource source;
	bool keepOldLogs;
} LogSettings;

// Struct storing the actual logger itelf.
typedef struct Logger_s
{
	LogSettings* settings;
	FILE* handle;
	bool initialised;
} Logger;

bool Logging_Init();
void Logging_LogChannel(const char* text, LogChannel channel, ...);
void Logging_LogAll(const char* text, ...);
void Logging_Shutdown();

Logger* Logger_new();

void Logger_destroy(Logger* logger);

LogSettings* LogSettings_new(const char* fileName, LogChannel channels, LogSource source, bool keepOldLogs);

void LogSettings_destroy(LogSettings* settings);

Logger* sys_logger;