// modern:personality Project (neé OpenWindows) Runtime
// 
// A compatibility layer for legacy MS-DOS and Multitasking MS-DOS apps intended to allow cross-compiling, in order to facilitate development on modern systems.
// Combines an 8086 emulator (not entirely accurate, designed to run as fast as possible) and trapping MS-DOS and MT-DOS APIs and thunking them to be used on the host system
// 
// © 2024 starfrost			Development started: January 7, 2024 
//							In September 2024 I got bored enough to try and finish this

#include "macros.h"
#include "cmd/cmd.h"
#include "dos/dos.h"
#include "cpu/8086/8086.h"
#include "util/logging.h"

#include <stdlib.h>

const char help_message[] =
{ 
	"dev16 Help\n\n" 
	"--REQUIRED USAGE--\n"
	"dev16 -dosver filename\n\n"
	"--OPTIONAL USAGE--\n"
	"dev16 [-mtdosver] filename\n\n"
	"--REQUIRED ARGUMENTS--\n"
	"-filename: Provides a .COM, .EXE (MZ or NE) file to run.\n"
	"-dosver: Provides a (non-multitasking) MS-DOS API level.\n\n"
	"Values:\n"
	"1: MS-DOS 1.x\t\tAlarmingly useless.\n"
	"2: MS-DOS 2.0\t\tSubstantially less useless - file handlers, drivers, IOCTL, etc.\n"
	"3: MS-DOS 2.11\t\tAdds INT 21h,AH=58h (Get/set memory allocation strategy)\n"
	"4: MS-DOS 2.25\t\tInternationalised DOS, with new country and language APIs\n"
	"5: MS-DOS 3.0\t\tNetwork redirection (unfinished), temp files, FLOCK, TRUENAME, more IOCTLs\n"
	"6: MS-DOS 3.1\t\tFinished network redirection, more IOCTLs\n"
	"7: MS-DOS 3.2\t\tEven more IOCTLs and internationalised stuff from DOS 2.25\n"
	"8: MS-DOS 3.3\t\tMore internationalisation, more handles, more good stuff\n"
	"9: MS-DOS 4.0\t\tFile commit, even more internationalisation, extended redirection, get boot drive, bugs, bloat\n"
	"10: MS-DOS 5.0\t\tLess bloat, DOS in HIGH, ENABLE/DISABLE drive, ROM-DOS, extended versioning information\n"
	"11: MS-DOS 6.0-6.2\tDisk compression, DBLSPACE APIs, upper memory optimisation\n"
	"12: MS-DOS 6.21\t\tNo DBLSPACE\n"
	"13: MS-DOS 6.22\t\tDRVSPACE not DBLSPACE (no hash tables...)\n\n"
	"--OPTIONAL ARGUMENTS--\n"
	"-mtdosver: Automatically turns on Multitasking MS-DOS 4.0 emulation. Determines the sub-version of Multitasking MS-DOS 4.0 to run.\n\n"
	"Values:\n"
	"1: Internal Work #2.06, 29 May 1984\t\t (\"M/T-MSDOS Beta Release 5/29/84)\n"
	"2: Build 4.11. 28 May 1985\t\t\t (internal beta - worse version of 6.7)\n"
	"3: Build 6.7, 26 November 1985\t\t\t - Preemptive multitasking, multiple screen devices, dynamic linking, process priority, session management, shared memory, semaphores, processes sharing code segments, etc etc etc etc\n"
	"4: Final release, 17 November 1986\t\t - Actually less stuff due to the fact the project was basically cancelled, no session management etc, seemingly incompatible NEs\n"
};

int main(int argc, char* argv[])
{
	Logging_Init();

	Logging_LogAll("dev16 v%s (%s %s)", X86_VERSION, __DATE__, __TIME__);
	Logging_LogAll("Develop for ancient computers on modern ones");
	Logging_LogAll("© 2023-2024 starfrost");

	if (!CMD_Parse(argc, argv))
	{
		//todo: print help
		Logging_LogChannel(help_message, LogChannel_Message);
		exit(1);
	}

	i8086_Init();

	bool os_init = false;

	if (cmd.is_mtdos)
	{
		os_init = MTDOS_Init();
	}
	else
	{
		os_init = MSDOS_Init();
	}

	if (!os_init)
	{
		exit(2);
	}

	// init complete, run
	machine_running = true;
	
	// machine_running checked inside of this func
	i8086_Update();

	return 0;
}
