#include "cpu/8086/8086.h"
#include "cpu/machine.h"
#include "dos.h"
#include "util/logging.h"

#include <stdbool.h>
#include <stdint.h>

// dos_int21.c : Traps MS-DOS Interrupt 21H api.

void MSDOS_Int21()
{
	basecpu_t* cpu = CPU_Get();

	Logging_LogChannel("INT 21 DISPATCHER ENTRY AH=%0x", LogChannel_Debug, cpu->AH);

	// TODO: INVALID FUNCTION HANDLER!!!!

	switch (cpu->AH)
	{
	case 0x00:					// Old exit (AH=00)
		MSDOS_Exit();
		break;
	case 0x01:
		MSDOS_ReadStdinEcho();	// Read stdin with echo
		break;
	case 0x06:
		MSDOS_WriteStdout();	// Write stdout or raw stdin (AH=06, depending on if DL=FFh)
		break;
	case 0x09:					// Print String (AH=09)
		MSDOS_PrintString();
		break;
	case 0x18:					// CP/M Compatibility Stub Functions
	case 0x1D:
	case 0x1E:
	case 0x20:
		cpu->AH = 0x00;
		break;

	case 0x30:
		MSDOS_GetVersion();
		break;
	case 0x4C:
		MSDOS_ExitWithExitCode();
		break;
	default:	// default behaviour for invalid function is to set AL to 0
		Logging_LogChannel("UNHANDLED int21 func %04Xh called (likely unimplemented or nonexistent!)", cpu->AH);
		cpu->AL = 0;
		return;

	}
}

void MSDOS_GetVersion()
{
	basecpu_t* cpu = CPU_Get();
	cpu->AL = 0x00; // DOS 1.X (due to invalid function code), PLACEHOLDER!!!
}

void MSDOS_Exit()
{
	basecpu_t* cpu = CPU_Get();

	// exit with exit code 0
	cpu->AL = 0x00;

	MSDOS_ExitWithExitCode();
}

void MSDOS_ExitWithExitCode()
{
	//PLACEHOLDER
	basecpu_t* cpu = CPU_Get();
	
	// debug message for successful exit
	if (cpu->AL == 0)
	{
		Logging_LogChannel("The application requested an exit with exit code 0.", LogChannel_Debug);
	}
	else
	{
		Logging_LogChannel("The application requested an exit with non-success exit code 0x%02X!.", LogChannel_Warning, cpu->AL);
	}

	machine_running = false;
}