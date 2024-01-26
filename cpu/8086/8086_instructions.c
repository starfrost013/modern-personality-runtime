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

i8086_modrm_t i8086_ModRM(bool w, uint8_t opcode, uint8_t modrm)
{
	// NOT VERIFIED!
	// MIGHT BE WRONG!

	i8086_modrm_t modrm_info;

	modrm_info.mod = (modrm) & 0xC0; // pull out bits 6-7
	uint8_t reg_temp = (modrm << 2) & 0xE0; // pull out bits 3-5
	modrm_info.rm = (modrm) & 0x07; // pull out bits 0-2

	// for some instructions these are extra opcodes
	modrm_info.ext_opcode = reg_temp;

	// Mode 0 - Memory Argument / General Register Argument
	// Mode 1 - Memory Argument / General Register Argument + 8-Bit Signed Displacement Address
	// Mode 2 - Memory Argument / Segment Register Argument + 16-Bit Signed Displacement Address
	// Mode 3 - Register Argument

	// determine the register we are modifying
	switch (reg_temp)
	{
	case 0:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.AX;

		}
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.AL;
		}
		break;
	case 1:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.CX;
		}
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.CL;
		}
		break;
	case 2:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.DX;
		}
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.DL;
		}
		break;
	case 3:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.BX;
		}
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.BL;
		}

		break;
	case 4:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.SP;
		}
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.AH;
		}
		break;
	case 5:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.BP;
		}
		else
		{
			 modrm_info.reg_ptr8 = &cpu_8086.CH;
		}
		break;
	case 6:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.SI;
		}
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.DH;
		}

		break;
	case 7:
		if (w)
		{
			modrm_info.reg_ptr16 = &cpu_8086.DI;
		} 
		else
		{
			modrm_info.reg_ptr8 = &cpu_8086.BH;

		}
		break;
	}

	uint8_t segreg_default = cpu_8086.address_space[cpu_8086.DS];

	switch (cpu_8086.last_prefix)
	{
	case override_ds: // todo: figure out which ones default to SS
		segreg_default = cpu_8086.address_space[cpu_8086.DS];
		break;

	case override_ss:
		segreg_default = cpu_8086.address_space[cpu_8086.SS];
		break;
	case override_es:
		segreg_default = cpu_8086.address_space[cpu_8086.ES];
		break;
	case override_cs:
		segreg_default = cpu_8086.address_space[cpu_8086.CS];
		break;
	default:
		// BP based indexing uses SS by default, for stack frames
		if (modrm_info.rm == 2
			|| modrm_info.rm == 3
			|| modrm_info.rm == 6)
		{
			segreg_default = &cpu_8086.address_space[cpu_8086.SS];
		}
		else
		{
			segreg_default = &cpu_8086.address_space[cpu_8086.DS];
		}
		break;
	}

	// make it actually based on segment registers
	segreg_default *= X86_PARAGRAPH_SIZE;

	if (modrm_info.mod == 0b11)
	{
		// completely differnet register order for fuck knows what reason
		// final offset is just a pointer so it modfies the ax reg by dereferencing it
		switch (modrm_info.rm)
		{
		case 0:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.AX;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.AL;
			}
			break;
		case 1:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.CX;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.CL;
			}
			break;
		case 2:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.DX;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.DL;
			}
			break;
		case 3:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.BX;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.BL;
			}
			break;
		case 4:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.SP;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.AH;
			}
			break;
		case 5:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.BP;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.CH;
			}
			break;
		case 6:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.SI;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.DH;
			}
		case 7:
			if (w)
			{
				modrm_info.final_offset = &cpu_8086.DI;
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.BH;
			}
			break;

		}
	}
	else
	{
		// switch the r/m bits
		switch (modrm_info.rm)
		{
			case 0:
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BX + cpu_8086.SI];
				break;
			case 1:
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BX + cpu_8086.DI];
				break;
			case 2: 
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BP + cpu_8086.SI];
				break;
			case 3:
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BP + cpu_8086.DI];
				break;
			case 4:
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.SI];
				break;
			case 5:
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.DI];
				break;
			case 6:
				// why
				if (modrm_info.mod == 0)
				{
					modrm_info.final_offset = &cpu_8086.address_space[segreg_default + i8086_ReadS16(cpu_8086._PC)];
					cpu_8086.IP += 2; // _PC updates every cycle but it's at the start so we update it here
					cpu_8086._PC += 2;

				}
				else
				{
					modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BP];
				}

				break;
			case 7:
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BX];
				break;
		}

		if (modrm_info.mod == 0b01)
		{
			modrm_info.final_offset += i8086_ReadS8(cpu_8086._PC);
			cpu_8086.IP++;
			cpu_8086._PC++;
		}
		else if (modrm_info.mod == 0b10)
		{
			modrm_info.final_offset += i8086_ReadS16(cpu_8086._PC);
			cpu_8086.IP += 2;
			cpu_8086._PC += 2;
		}

	}

	return modrm_info;
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