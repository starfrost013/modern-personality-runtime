#pragma once
#include "8086.h"
// 8086_instructions.c : Implements some instructions that are too big for the regular giant switch statement.


void i8086_Add8(uint8_t* destination, uint8_t source, bool adc)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the addition
	*destination = source + *destination;

	// save the final value
	uint8_t final_value = *destination;

	i8086_SetOF8(final_value, source, original_value, false); // we are not subtracting
	i8086_SetCF8(final_value);
	i8086_SetAF8(final_value, source, original_value);
	i8086_SetZF8(final_value);
	i8086_SetPF8(final_value);
	i8086_SetSF8(final_value);

	if (adc)
	{
		if (cpu_8086.flag_carry)
		{
			// increment the destination value
			*destination++;
		}
	}
}

void i8086_Add16(uint16_t* destination, uint16_t source, bool adc)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the addition
	*destination = source + *destination;

	// save the final value
	uint8_t final_value = *destination;

	i8086_SetOF16(final_value, source, original_value, false); // we are not subtracting
	i8086_SetCF16(final_value);
	i8086_SetAF16(final_value, source, original_value);
	i8086_SetZF16(final_value);
	i8086_SetPF16(final_value);
	i8086_SetSF16(final_value);

	if (adc)
	{
		if (cpu_8086.flag_carry)
		{
			// increment the destination value
			*destination++;
		}
	}
}

void i8086_Loop(uint8_t destination_offset, bool condition)
{
	cpu_8086.CX--;

	// 0xE0-0xE3
	if (condition)
	{
		cpu_8086.IP += destination_offset;
	}
	else
	{
		// move CPU on to the next instruction
		cpu_8086.IP++;
	}
}

void i8086_JumpConditional(uint8_t destination_offset, bool condition)
{
	if (condition)
	{
		cpu_8086.IP += destination_offset;
	}
	else
	{
		// onto next instruction
		cpu_8086.IP++;
	}
}

void i8086_Push(uint16_t value)
{
	// stack grows DOWN!!!!!
	cpu_8086.SP -= 2;
	cpu_8086.address_space[cpu_8086._realSP] = value & 0x00FF;
	cpu_8086.address_space[cpu_8086._realSP + 1] = value >> 8;
}

uint16_t i8086_Pop()
{
	uint16_t ret_val = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
	cpu_8086.SP += 2;
	return ret_val;
}

void i8086_MoveSegOff8(uint8_t value, bool direction)
{
	// modrm but for some reason both mod and reg are avoided, so it has to have its own implementation only for opcodes a0-a3. also only for the AH register.

	// default register for this is DS
	uint16_t* seg_ptr = &cpu_8086.address_space[(cpu_8086.DS * X86_PARAGRAPH_SIZE) + value];

	switch (cpu_8086.last_prefix)
	{
		case override_es:
			seg_ptr = &cpu_8086.address_space[(cpu_8086.ES * X86_PARAGRAPH_SIZE) + value];
			Logging_LogChannel("MOV AL, [ES:%02x]", value);
			break;
		case override_cs:
			seg_ptr = &cpu_8086.address_space[(cpu_8086.CS * X86_PARAGRAPH_SIZE) + value];
			Logging_LogChannel("I don't think this is a great idea (MOV AL, [CS:%02x])", LogChannel_Warning, value);
			break;
		case override_ss:
			seg_ptr = &cpu_8086.address_space[(cpu_8086.SS * X86_PARAGRAPH_SIZE) + value];
			Logging_LogChannel("MOV AL, [SS:%02x]", LogChannel_Warning, value);
			break;
	}


	if (!direction) // a0-a1
	{
		cpu_8086.AL = *seg_ptr;
	}
	else // a2-a3
	{
		*seg_ptr = cpu_8086.AL;
	}
}

void i8086_MoveSegOff16(uint16_t value, bool direction)
{
	// modrm but for some reason both mod and reg are avoided, so it has to have its own implementation only for opcodes a0-a3. also only for the AH register.

	// default register for this is DS
	uint16_t* seg_ptr = &cpu_8086.address_space[(cpu_8086.DS * X86_PARAGRAPH_SIZE) + value];

	switch (cpu_8086.last_prefix)
	{
	case override_es:
		seg_ptr = &cpu_8086.address_space[(cpu_8086.ES * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("MOV AL, [ES:%02x]", value);
		break;
	case override_cs:
		seg_ptr = &cpu_8086.address_space[(cpu_8086.CS * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("I don't think this is a great idea (MOV AL, [CS:%02x])", LogChannel_Warning, value);
		break;
	case override_ss:
		seg_ptr = &cpu_8086.address_space[(cpu_8086.SS * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("MOV AL, [SS:%02x]", LogChannel_Warning, value);
		break;
	}


	if (!direction) // a0-a1
	{
		cpu_8086.AX = *seg_ptr;
	}
	else // a2-a3
	{
		*seg_ptr = cpu_8086.AX;
	}
}