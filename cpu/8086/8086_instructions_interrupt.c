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

	// if the IVT isn't zero
	if (i8086_GetInterruptVector(interrupt_num) > 0x0) 
	{
		// get the cs & ip
		uint16_t cs = (interrupt_num >> 16); // drop low bits
		uint16_t ip = (interrupt_num << 16) & 0xFFFF; // drop high bits and mask

		// push ojump and return

		// CS is pushed first...

		i8086_Push(i8086_FlagsToWord());
		i8086_Push(cpu_8086.CS);
		i8086_Push(cpu_8086.IP);

		cpu_8086.CS = cs;
		cpu_8086.IP = ip;
		return;
	}

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
	case MSDOS_INTERRUPT_API:	// General DOS API dispatcher.
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

// Get an interrupt vector from the IVT.
// Allows applications to override the interrupt vectors we provide

uint32_t i8086_GetInterruptVector(uint8_t interrupt_vector)
{
	// little endian

	uint32_t ivt_entry_location = (interrupt_vector) * 4;

	uint16_t ip = *(uint16_t*)&cpu_8086.address_space[ivt_entry_location];
	uint16_t cs = *(uint16_t*)&cpu_8086.address_space[ivt_entry_location + 2];

	// *X86_PARAGRAPH_SIZE

	// << 4 would be faster, but less readable - see later if performance loss matters enough
	return (cs * X86_PARAGRAPH_SIZE) + ip;
}