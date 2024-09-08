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
	case 0x00:				// Old exit (AH=00)
		MSDOS_Exit();
		break;
	case 0x09:				// Print String (AH=09)
		MSDOS_PrintString();
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

void MSDOS_PrintString()
{
	basecpu_t* cpu = CPU_Get();

	uint32_t start_location = (cpu->DS) * X86_PARAGRAPH_SIZE + cpu->DX;

	// TODO: INT23 CHECK!!!!!

	// TEMP code
	if (cmd.cpu_ver == cpu_type_i8086)
	{
#if X86_DEBUG
		int32_t debug_count = 0x00;
#endif
		char* next_char = &cpu_8086.address_space[start_location];

		while (*next_char != '$')
		{
			putchar(*next_char);
			// go to next byte
			next_char++;
#if X86_DEBUG
			debug_count++;
			if (debug_count > 8192)
			{
				Logging_LogChannel("***DEBUG WARNING***: TRIED TO PRINT OVER 8KB OF TEXT via INT 21h,AH=09h something is going very wrong", LogChannel_Warning);
			}
#endif
		}
	}

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