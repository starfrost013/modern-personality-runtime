#include "cpu/8086/8086.h"
#include "cpu/machine.h"
#include "dos.h"
#include "util/logging.h"

#include <stdbool.h>
#include <stdint.h>

// dos_int21_string.c: Handles int21, ah=01-09 (STDIN/STDOUT/STDAUX)

void MSDOS_ReadStdinRaw()
{
	cpu_8086.AL = getc(stdin);
}

void MSDOS_ReadStdin()
{
	MSDOS_ReadStdinRaw();

	// check for ^C/^BREAK and issue INT23 if we did that...
	if (cpu_8086.AL == 0x03				//Ctrl-C
		|| cpu_8086.AL == 0x1A)			//Ctrl-Z / Ctrl-Break
	{
		// this function needs to be genericised
		i8086_InterruptForce(0x23);
	}
}

void MSDOS_ReadStdinEcho()
{
	MSDOS_ReadStdin();
	
	//echo to display
	printf((char)cpu_8086.AL);
}

void MSDOS_WriteStdout()
{
	basecpu_t* cpu_generic = CPU_Get();

	printf((char)cpu_generic->DL);

	// docs say nothing is returned but they are wrong
	// MOV     AL,DL
	// Maybe they intended to return nothing in AL, but they needed it for a temp register...
	// https://github.com/microsoft/MS-DOS/blob/main/v1.25/source/MSDOS.ASM

	cpu_generic->AL = cpu_generic->DL;			// MOV	AL, DL	

	if (cpu_generic->DL != 0xFF)			// CMP	AL,-1
		printf((char)cpu_generic->DL);		// JNZ  RAWOUT
	else
		MSDOS_ReadStdinRaw();			
}


void MSDOS_ReadStdinRawEcho()
{

}

void MSDOS_PrintString()
{
	basecpu_t* cpu_generic = CPU_Get();

	uint32_t start_location = (cpu_generic->DS) * X86_PARAGRAPH_SIZE + cpu_generic->DX;

	// TODO: INT23 CHECK!!!!!

	// TEMP code
	if (cmd.cpu_ver == cpu_type_i8086)
	{
		int32_t debug_count = 0x00;
		char* next_char = &cpu_8086.address_space[start_location];

		while (*next_char != '$')
		{
			putchar(*next_char);
			// go to next byte
			next_char++;
			debug_count++;

			if (debug_count > 8192)
				Logging_LogChannel("***DEBUG WARNING***: Tried to print a ridiculous amount (>8192 bytes) of text using INT 21h,AH=09h something is going very wrong", LogChannel_Warning);
		}
	}

}