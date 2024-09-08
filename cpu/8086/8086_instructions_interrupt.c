#include "8086.h"
#include "util/logging.h"
#include "cpu/machine.h"

#include <stdint.h>
#include <stdbool.h>

// This code is a part of the blah blah distribution
//
// 8086_instructions_interrupt.c: Handle interrupts.
// 9 September 2024

void i8086_InterruptService(uint8_t interrupt_num)
{
	Logging_LogChannel("The application called an interrupt service routine.\n\n"
		"INT %02Xh AX=%04Xh BX=%04Xh CX=%04Xh DX=%04Xh\n"
		"CS=%04Xh IP=%04Xh (Physical address of PC=%05X) DS=%04Xh ES=%04Xh SS=%04Xh\n"
		"BP=%04Xh DI=%04Xh SI=%04Xh", LogChannel_Debug, interrupt_num,
		cpu_8086.AX, cpu_8086.BX, cpu_8086.CX, cpu_8086.DX, cpu_8086.CS, cpu_8086.IP, cpu_8086._PC, cpu_8086.DS, cpu_8086.ES, cpu_8086.SS,
		cpu_8086.BP, cpu_8086.DI, cpu_8086.ES);

	// there is no IVT yet...
	switch (interrupt_num)
	{
	// these could be handled by the general case but I want to log when they get called
	// INT 07h+ are 286+ and so far out of scope
	case 0x00:
		Logging_LogChannel("INT 00h division by zero. Unimplemented.", LogChannel_Error, interrupt_num);
		break;
	case 0x01:
		Logging_LogChannel("INT 01h single step. Unimplemented.", LogChannel_Error, interrupt_num);
		break;
	case 0x02:
		Logging_LogChannel("INT 02h NMI. THIS SHOULD NEVER HAPPEN, because it comes from external hardware only.", LogChannel_Error, interrupt_num);
		break;
	case 0x03:
		Logging_LogChannel("INT 03h (not an INT3 instruction). THIS SHOULD NEVER HAPPEN", LogChannel_Error, interrupt_num);
		break;
	case 0x04:
		Logging_LogChannel("INT 04h INTO overflow. Unimplemented.", LogChannel_Error, interrupt_num);
		break;
	case 0x05:
		Logging_LogChannel("INT 05h (print, 186+ bound exceeded). Unimplemented.", LogChannel_Error, interrupt_num);
		break;
	case 0x06:
		if (cmd.cpu_ver == cpu_type_i8086)
			Logging_LogChannel("INT 06h. THIS SHOULD NEVER HAPPEN", LogChannel_Error, interrupt_num);
		else
			Logging_LogChannel("Invalid opcode handler not implemented", LogChannel_Error, interrupt_num); //186

		break;
	case 0x21:				// General DOS API dispatcher.
		MSDOS_Int21();
		break;
	default:
		// nothing to do beyond here the program will crash anyway 
		Logging_LogChannel("UNHANDLED int %04Xh called (likely unimplemented)", LogChannel_Fatal, interrupt_num);
		break;
	}

	cpu_8086.IP += 2;
}

void i8086_Interrupt()
{
	uint8_t interrupt_num = i8086_ReadU8(cpu_8086._PC); //increment IP and read 
	cpu_8086._PC++;
	i8086_InterruptService(interrupt_num);
}

// In some cases, an interrupt can occur during an instruction in a form of "pseudo-exception"
void i8086_InterruptForce(uint8_t interrupt_num)
{
	i8086_InterruptService(interrupt_num);
}