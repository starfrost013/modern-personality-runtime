#pragma once

#include "..\core.h"
#include "..\dos\dos.h"

// CMD.h: parses command line and stores program state.

// Type of cpu to initialise.
typedef enum cpu_type_e
{
	cpu_type_i8086 = 0,
} cpu_type;

// Command-line information structure.
typedef struct cmd_s
{
	cpu_type		cpu_ver;						// CPU version (8086)
	dos_api_level	msdos_ver;						// MS-DOS version
	mtdos_api_level	mtdos_ver;						// MT-DOS version
	bool			is_mtdos;						// True = MT-DOS/MDOS4, False = regular dos
	char			command_line[MAX_PATH_MODERN];	// Command line
	binary_type		binary_type;					// The binary type
	FILE*			handle;							// Handle to the binary.
} cmd_t;

cmd_t cmd;

bool CMD_Parse(int argc, char* argv[]);