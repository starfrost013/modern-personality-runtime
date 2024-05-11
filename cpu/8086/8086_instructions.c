#include "8086.h"
#include "util/logging.h"
#include <string.h>

// 8086_instructions.c : Implements some instructions that are too big for the regular giant switch statement.

void i8086_Add8(uint8_t* destination, uint8_t* source, bool adc)
{
	uint8_t original_value = 0;
	// so the original value doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the addition
	*destination = *source + *destination;

	// save the final value
	uint8_t final_value = *destination;

	i8086_SetOF8(final_value, *source, original_value, false); // we are not subtracting
	i8086_SetCF8(final_value);
	i8086_SetAF8(final_value, *source, original_value);
	i8086_SetZF8(final_value);
	i8086_SetPF8(final_value);
	i8086_SetSF8(final_value);

	if (adc)
	{
		if (cpu_8086.flag_carry)
		{
			// increment the destination value
			(*destination)++;
		}
	}
}

void i8086_Add16(uint16_t* destination, uint16_t* source, bool adc)
{
	uint8_t original_value = 0;
	// so the original value doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the addition
	*destination = *source + *destination;

	// save the final value
	uint16_t final_value = *destination;

	i8086_SetOF16(final_value, *source, original_value, false); // we are not subtracting
	i8086_SetCF16(final_value);
	i8086_SetAF16(final_value, *source, original_value);
	i8086_SetZF16(final_value);
	i8086_SetPF16(final_value);
	i8086_SetSF16(final_value);

	if (adc)
	{
		if (cpu_8086.flag_carry)
		{
			// increment the destination value
			(*destination)++;
		}
	}
}


void i8086_Sub8(uint8_t* destination, uint8_t* source, bool sbb)
{
	uint8_t original_value = 0;
	// so the original value doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the subtraction

	if (sbb)
	{
		*destination = *destination - (*source + cpu_8086.flag_carry);
	}
	else
	{
		*destination = *destination - *source;
	}

	// save the final value
	uint8_t final_value = *destination;

	i8086_SetOF8(final_value, *source, original_value, false); // we are not subtracting
	i8086_SetCF8(final_value);
	i8086_SetAF8(final_value, *source, original_value);
	i8086_SetZF8(final_value);
	i8086_SetPF8(final_value);
	i8086_SetSF8(final_value);

}

void i8086_Sub16(uint16_t* destination, uint16_t* source, bool sbb)
{
	uint16_t original_value = 0;
	// so the original value doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint16_t));

	// do the subtraction
	if (sbb)
	{
		*destination = *destination - (*source + cpu_8086.flag_carry);
	}
	else
	{
		*destination = *destination - *source;
	}

	// save the final value
	uint16_t final_value = *destination;

	i8086_SetOF16(final_value, *source, original_value, false); // we are not subtracting
	i8086_SetCF16(final_value);
	i8086_SetAF16(final_value, *source, original_value);
	i8086_SetZF16(final_value);
	i8086_SetPF16(final_value);
	i8086_SetSF16(final_value);
}

void i8086_Cmp8(uint8_t* destination, uint8_t* source)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// CMP is the same as SUB, but it doesn't save the final result and just sets flags
	uint8_t final_value = *destination - *source;

	i8086_SetOF8(final_value, *source, original_value, false); // we are not subtracting
	i8086_SetCF8(final_value);
	i8086_SetAF8(final_value, *source, original_value);
	i8086_SetZF8(final_value);
	i8086_SetPF8(final_value);
	i8086_SetSF8(final_value);
}

void i8086_Cmp16(uint16_t* destination, uint16_t* source)
{
	uint16_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint16_t));

	// CMP is the same as SUB, but it doesn't save final result and just sets flags
	uint16_t final_value = *destination - *source;

	// cmp doesn't save the final result and just sets flags
	i8086_SetOF16(final_value, *source, original_value, false); // we are not subtracting
	i8086_SetCF16(final_value);
	i8086_SetAF16(final_value, *source, original_value);
	i8086_SetZF16(final_value);
	i8086_SetPF16(final_value);
	i8086_SetSF16(final_value);
}

void i8086_Or8(uint8_t* destination, uint8_t* source)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	*destination = *destination | *source;

	i8086_SetZF8(*destination);
	i8086_SetPF8(*destination);
	i8086_SetSF8(*destination);
}

void i8086_Or16(uint16_t* destination, uint16_t* source)
{
	uint16_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint16_t));

	*destination = *destination | *source;

	// cmp doesn't save the final result and just sets flags
	// OF and CF are always zero
	cpu_8086.flag_carry = false;
	cpu_8086.flag_overflow = false;

	i8086_SetZF16(*destination);
	i8086_SetPF16(*destination);
	i8086_SetSF16(*destination);
}

void i8086_And8(uint8_t* destination, uint8_t* source)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	*destination = *destination & *source;

	i8086_SetZF8(*destination);
	i8086_SetPF8(*destination);
	i8086_SetSF8(*destination);
}

void i8086_And16(uint16_t* destination, uint16_t* source)
{
	uint16_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	*destination = *destination & *source;

	// cmp doesn't save the final result and just sets flags
	// OF and CF are always zero
	cpu_8086.flag_carry = false;
	cpu_8086.flag_overflow = false;

	i8086_SetZF16(*destination);
	i8086_SetPF16(*destination);
	i8086_SetSF16(*destination);
}

void i8086_Xor8(uint8_t* destination, uint8_t* source)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	*destination = *destination ^ *source;

	i8086_SetZF8(*destination);
	i8086_SetPF8(*destination);
	i8086_SetSF8(*destination);
}

void i8086_Xor16(uint16_t* destination, uint16_t* source)
{
	uint16_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	*destination = *destination ^ *source;

	// cmp doesn't save the final result and just sets flags
	// OF and CF are always zero
	cpu_8086.flag_carry = false;
	cpu_8086.flag_overflow = false;

	i8086_SetZF16(*destination);
	i8086_SetPF16(*destination);
	i8086_SetSF16(*destination);
}

// SAL DOES NOT EXIST ON 8086!
void i8086_Shl8(uint8_t* destination, uint8_t amount)
{
	uint8_t original_value = *destination;

	*destination = *destination << amount;

	if (amount == 1)
	{
		// OF set if top two bits are identical
		cpu_8086.flag_carry = (original_value & 0x80);

		// why is this here??? we'll find out
		cpu_8086.flag_overflow = ((amount ^ *destination) & 0x80) == cpu_8086.flag_carry;
	}

	// set flags
	i8086_SetSF8(*destination);
	i8086_SetPF8(*destination);
	i8086_SetZF8(*destination);
	cpu_8086.flag_aux_carry = true;
}

void i8086_Shl16(uint16_t* destination, uint8_t amount)
{
	uint16_t original_value = *destination;

	*destination = *destination << amount;

	if (amount == 1)
	{
		// OF set if top two bits are identical
		cpu_8086.flag_carry = (original_value & 0x80);

		// why is this here??? we'll find out
		cpu_8086.flag_overflow = ((amount ^ *destination) & 0x80) == cpu_8086.flag_carry;
	}

	// set flags
	i8086_SetSF16(*destination);
	i8086_SetPF16(*destination);
	i8086_SetZF16(*destination);
	cpu_8086.flag_aux_carry = true;
}

void i8086_Shr8(uint8_t* destination, uint8_t amount, bool sar)
{
	uint8_t original_value = *destination;
	bool original_sign = false;  // ignored on SHR

	if (sar)
	{
		original_sign = (sar & 0x80);
	}

	*destination = *destination >> amount;

	if (amount == 1)
	{
		// OF set if top two bits are identical
		cpu_8086.flag_carry = (original_value & 0x01);

		// why is this here??? we'll find out
		cpu_8086.flag_overflow = (*destination & 0x80) == cpu_8086.flag_carry;
	}

	// restore the original sign
	if (sar)
	{
		//true=negative
		//false=positive
		if (original_sign
			&& *destination < 0x80) *destination += 0x80;
	}

	// set flags
	i8086_SetSF8(*destination);
	i8086_SetPF8(*destination);
	i8086_SetZF8(*destination);
	cpu_8086.flag_aux_carry = true;
}

void i8086_Shr16(uint16_t* destination, uint8_t amount, bool sar)
{
	uint16_t original_value = *destination;

	*destination = *destination >> amount;

	if (amount == 1)
	{
		// OF set if top two bits are identical
		cpu_8086.flag_carry = (original_value & 0x01);

		// why is this here??? we'll find out
		cpu_8086.flag_overflow = (*destination & 0x80) == cpu_8086.flag_carry;
	}

	// set flags
	i8086_SetSF16(*destination);
	i8086_SetPF16(*destination);
	i8086_SetZF16(*destination);
	cpu_8086.flag_aux_carry = true;
}

void i8086_Rol8(uint8_t* destination, uint8_t amount, bool rcl)
{
	uint8_t original_value = *destination;

	for (int32_t n = 0; n < amount; n++)
	{
		bool msb = false;

		if (rcl)
		{
			msb = (*destination & 0x80);
			*destination = ((*destination << amount) << 1);
			if (msb) (*destination)++;
		}
		else
		{
			msb = (*destination & 0x80);
			*destination = (*destination << amount) << 1;
			cpu_8086.flag_carry = msb;
			if (msb) (*destination)++;
		}
		// get the most significant bit and make it the least significant

	}

	cpu_8086.flag_carry = (*destination & 0x01);

	if (amount == 1)
	{
		// OF set if top two bits are identical
		// why is this here??? we'll find out
		cpu_8086.flag_overflow = ((amount ^ cpu_8086.flag_carry) & 0x80) == cpu_8086.flag_carry;
	}
}

void i8086_Rol16(uint16_t* destination, uint8_t amount, bool rcl)
{
	uint16_t original_value = *destination;

	for (int32_t n = 0; n < amount; n++)
	{
		bool msb = false;

		if (rcl)
		{
			msb = (*destination & 0x8000);
			*destination = ((*destination << amount) << 1) + cpu_8086.flag_carry;
			cpu_8086.flag_carry = msb;
			if (msb) (*destination)++;
		}
		else
		{
			msb = (*destination & 0x8000);
			*destination = (*destination << amount) << 1;

			if (msb) (*destination)++;
		}
		// get the most significant bit and make it the least significant

	}

	cpu_8086.flag_carry = (*destination & 0x01);

	if (amount == 1)
	{
		// OF set if top two bits are identical
		// why is this here??? we'll find out
		cpu_8086.flag_overflow = ((amount ^ cpu_8086.flag_carry) & 0x8000) == cpu_8086.flag_carry;
	}
}

void i8086_Ror8(uint8_t* destination, uint8_t amount, bool rcr)
{
	uint8_t original_value = *destination;

	if (rcr)
	{
		cpu_8086.flag_overflow = ((amount ^ cpu_8086.flag_carry) & 0x80) == cpu_8086.flag_carry;
	}

	for (int32_t n = 0; n < amount; n++)
	{
		bool lsb = false;

		// get the least significant bit and make it the most significant

		if (rcr)
		{
			lsb = (*destination & 0x01);
			*destination = ((*destination >> amount) >> 1) + (cpu_8086.flag_carry << 7);
			cpu_8086.flag_carry = lsb;
			if (lsb) (*destination)++;
		}
		else
		{
			lsb = (*destination & 0x01);
			*destination = ((*destination >> amount) >> 1) + (lsb << 7);
			if (lsb) (*destination)++;
		}

	}

	// OF set if top two bits are identical
	cpu_8086.flag_carry = (original_value & 0x80);

	if (!rcr
		&& amount == 1)
	{
		// why is this here??? we'll find out
		cpu_8086.flag_overflow = ((amount ^ cpu_8086.flag_carry) & 0x80) == cpu_8086.flag_carry;
	}
}

void i8086_Ror16(uint16_t* destination, uint8_t amount, bool rcr)
{
	uint16_t original_value = *destination;

	if (rcr)
	{
		cpu_8086.flag_overflow = ((amount ^ cpu_8086.flag_carry) & 0x80) == cpu_8086.flag_carry;
	}

	for (int32_t n = 0; n < amount; n++)
	{
		bool lsb = false;

		// get the least significant bit and make it the most significant

		if (rcr)
		{
			lsb = (*destination & 0x01);
			*destination = ((*destination >> amount) >> 1) + (cpu_8086.flag_carry << 7);
			cpu_8086.flag_carry = lsb;
			if (lsb) (*destination)++;
		}
		else
		{
			lsb = (*destination & 0x01);
			*destination = ((*destination >> amount) >> 1) + (lsb << 7);
			if (lsb) (*destination)++;
		}
	}

	// OF set if top two bits are identical
	cpu_8086.flag_carry = (original_value & 0x8000);

	if (!rcr
		&& amount == 1)
	{
		// why is this here??? we'll find out
		cpu_8086.flag_overflow = ((amount ^ cpu_8086.flag_carry) & 0x8000) == cpu_8086.flag_carry;

	}
}


void i8086_Loop(uint8_t destination_offset, bool condition)
{
	cpu_8086.CX--;

	// 0xE0-0xE3
	if (condition)
	{
		// this saves code by casting to int8_t after to handle jumps backwards
		cpu_8086.IP += (int8_t)(destination_offset + 2); // relative to NEXT instruction
	}
	else
	{
		// move CPU on to the next instruction
		cpu_8086.IP += 2;
	}
}

void i8086_JumpConditional(uint8_t destination_offset, bool condition)
{
	if (condition)
	{
		// this saves code by casting to int8_t after to handle jumps backwards
		cpu_8086.IP += (int8_t)(destination_offset + 2); // relative to NEXT instruction
	}
	else
	{
		// onto next instruction
		cpu_8086.IP += 2;
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
	cpu_8086.SP += 2;
	cpu_8086._realSP += 2;
	uint16_t ret_val = (cpu_8086.address_space[cpu_8086._realSP + 1] << 8) + cpu_8086.address_space[cpu_8086._realSP];
	return ret_val;
}

i8086_modrm_t i8086_ModRM(uint8_t opcode, uint8_t modrm)
{
	// NOT VERIFIED!
	// MIGHT BE WRONG!

	i8086_modrm_t modrm_info = { 0 };
	bool direction = ((opcode >> 1) & 0x01); // pull out bit 1 of opcode for direction flag
	bool w = false; // false = 8-bit word length for oeprands, true=16-bit

	// HACK warning: for some reason, the reg field means something entirely different for SPECIFICALLY these two instructions. Fuck Intel!
	// They are always 16-bit, even though the w-field of the opcod register indicates the precise opposite, and the reg field selects segment registers
	// using an entirely different coding
	// INTEL? MORE LIKE UnIntelligent
	bool intel_moron_instructions = (opcode == 0x8C
		|| opcode == 0x8E);

	if (intel_moron_instructions)
	{
		w = true; // they are encoded as 8-bit but take 16-bit segment registers
	}
	else
	{
		w = (opcode) & 0x01;
	}

	modrm_info.mod = (modrm >> 6); // pull out bits 6-7
	uint8_t reg_temp = ((uint8_t)(modrm << 2) >> 5); // pull out bits 3-5
	modrm_info.rm = (modrm) & 0x07; // pull out bits 0-2

	// for some instructions these are extra opcodes
	modrm_info.ext_opcode = reg_temp;

	// Mode 0 - Memory Argument / General Register Argument
	// Mode 1 - Memory Argument / General Register Argument + 8-Bit Signed Displacement Address
	// Mode 2 - Memory Argument / Segment Register Argument + 16-Bit Signed Displacement Address
	// Mode 3 - Register Argument

#if X86_DEBUG
	char disassembly_string[DISASM_STR_SIZE] = { 0 };
	char operand1_string[DISASM_STR_SIZE] = { 0 };
	char operand2_string[DISASM_STR_SIZE] = { 0 };
#endif

	// 8c and 8e segment-register moves.
	if (intel_moron_instructions)
	{
		// appears to be simply masked off for invalid options
		reg_temp = reg_temp & 0x03;
		switch (reg_temp)
		{
		case 0:
			modrm_info.reg_ptr16 = &cpu_8086.ES;
#if X86_DEBUG
			strncat(operand1_string, "ES", 2);
#endif
			break;
		case 1:
			modrm_info.reg_ptr16 = &cpu_8086.CS;
#if X86_DEBUG
			strncat(operand1_string, "CS", 2);
#endif
			break;
		case 2:
			modrm_info.reg_ptr16 = &cpu_8086.SS;
#if X86_DEBUG
			strncat(operand1_string, "SS", 2);
#endif
			break;
		case 3:
			modrm_info.reg_ptr16 = &cpu_8086.DS;
#if X86_DEBUG
			strncat(operand1_string, "DS", 2);
#endif
			break;
		}
	}
	else // normally encoded instructions
	{
		// determine the register we are modifying
		switch (reg_temp)
		{
		case 0:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.AX;
#if X86_DEBUG
				strncat(operand1_string, "AX", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.AL;
#if X86_DEBUG
				strncat(operand1_string, "AL", 2);
#endif
			}
			break;
		case 1:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.CX;
#if X86_DEBUG
				strncat(operand1_string, "CX", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.CL;
#if X86_DEBUG
				strncat(operand1_string, "CL", 2);
#endif
			}
			break;
		case 2:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.DX;
#if X86_DEBUG
				strncat(operand1_string, "DX", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.DL;
#if X86_DEBUG
				strncat(operand1_string, "DL", 2);
#endif
			}
			break;
		case 3:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.BX;
#if X86_DEBUG
				strncat(operand1_string, "BX", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.BL;
#if X86_DEBUG
				strncat(operand1_string, "BL", 2);
#endif
			}

			break;
		case 4:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.SP;
#if X86_DEBUG
				strncat(operand1_string, "SP", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.AH;
#if X86_DEBUG
				strncat(operand1_string, "AH", 2);
#endif
			}
			break;
		case 5:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.BP;
#if X86_DEBUG
				strncat(operand1_string, "BP", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.CH;
#if X86_DEBUG
				strncat(operand1_string, "CH", 2);
#endif
			}
			break;
		case 6:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.SI;
#if X86_DEBUG
				strncat(operand1_string, "SI", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.DH;
#if X86_DEBUG
				strncat(operand1_string, "DH", 2);
#endif
			}

			break;
		case 7:
			if (w)
			{
				modrm_info.reg_ptr16 = &cpu_8086.DI;
#if X86_DEBUG
				strncat(operand1_string, "DI", 2);
#endif
			}
			else
			{
				modrm_info.reg_ptr8 = &cpu_8086.BH;
#if X86_DEBUG
				strncat(operand1_string, "BH", 2);
#endif

			}
			break;
		}
	}


	if (modrm_info.mod != 0b11)
	{
		// don't bother setting segreg_default if mod == 0b11 (register)
		uint8_t segreg_default = cpu_8086.address_space[cpu_8086.DS];

		switch (cpu_8086.last_prefix)
		{
		case override_ds: // todo: figure out which ones default to SS
			segreg_default = cpu_8086.address_space[cpu_8086.DS];
#if X86_DEBUG
			strncat(operand2_string, "DS:", 3);
#endif
			break;

		case override_ss:
			segreg_default = cpu_8086.address_space[cpu_8086.SS];
#if X86_DEBUG
			strncat(operand2_string, "SS:", 3);
#endif
			break;
		case override_es:
			segreg_default = cpu_8086.address_space[cpu_8086.ES];
#if X86_DEBUG
			strncat(operand2_string, "ES:", 3);
#endif
			break;
		case override_cs:
			segreg_default = cpu_8086.address_space[cpu_8086.CS];
#if X86_DEBUG
			strncat(operand2_string, "CS:", 3);
#endif
			break;
		default:
			// BP based indexing uses SS by default, for stack frames
			if (modrm_info.rm == 2
				|| modrm_info.rm == 3
				|| (modrm_info.rm == 6 && modrm_info.mod != 0)) // rm=6,mod=0 hardcoded to add a disp16 in microcode.
			{
				segreg_default = cpu_8086.address_space[cpu_8086.SS];
#if X86_DEBUG
				// decided to make it always obvious which segment is being used
				strncat(operand2_string, "SS:", 3);
#endif
			}
			else
			{
				segreg_default = cpu_8086.address_space[cpu_8086.DS];
#if X86_DEBUG
				// decided to make it always obvious which segment is being used
				strncat(operand2_string, "DS:", 3);
#endif
			}
			break;
		}

		// make it actually based on segment registers
		segreg_default *= X86_PARAGRAPH_SIZE;

		// switch the r/m bits
		switch (modrm_info.rm)
		{
		case 0:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BX + cpu_8086.SI];
#if X86_DEBUG
			strncat(operand2_string, "[BX+SI", 6);
#endif
			break;
		case 1:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BX + cpu_8086.DI];
#if X86_DEBUG
			strncat(operand2_string, "[BX+DI", 6);
#endif
			break;
		case 2:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BP + cpu_8086.SI];
#if X86_DEBUG
			strncat(operand2_string, "[BP+SI", 6);
#endif
			break;
		case 3:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BP + cpu_8086.DI];
#if X86_DEBUG
			strncat(operand2_string, "[BP+DI", 6);
#endif
			break;
		case 4:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.SI];
#if X86_DEBUG
			strncat(operand2_string, "[BP+SI", 6);
#endif
			break;
		case 5:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.DI];
#if X86_DEBUG
			strncat(operand2_string, "[BP+DI", 6);
#endif
			break;
		case 6:
			// why
			if (modrm_info.mod == 0)
			{
				uint16_t offset = i8086_ReadS16(cpu_8086._PC);
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + offset];
				cpu_8086.IP += 2; // _PC updates every cycle but it's at the start so we update it here
				cpu_8086._PC += 2;
#if X86_DEBUG
				char temp_offset[8] = { 0 };
				sprintf(temp_offset, "[%04Xh", offset);
				strncat(operand2_string, temp_offset, 8);
#endif

			}
			else
			{
				modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BP];
#if X86_DEBUG
				strncat(operand2_string, "[BP", 3);
#endif
			}

			break;
		case 7:
			modrm_info.final_offset = &cpu_8086.address_space[segreg_default + cpu_8086.BX];
#if X86_DEBUG
			strncat(operand2_string, "[BX", 3);
#endif
			break;
		}

		if (modrm_info.mod == 0b01)
		{
			uint8_t offset_8 = i8086_ReadS8(cpu_8086._PC);
			modrm_info.final_offset += offset_8;
			cpu_8086.IP++;
			cpu_8086._PC++;
#if X86_DEBUG
			char temp_string_offset8[12];

			sprintf(temp_string_offset8, "[%04Xh", offset_8);
			strncat(operand2_string, temp_string_offset8, 12);
#endif

		}
		else if (modrm_info.mod == 0b10)
		{
			uint16_t offset_16 = i8086_ReadS16(cpu_8086._PC);
			modrm_info.final_offset += offset_16;
			cpu_8086.IP += 2;
			cpu_8086._PC += 2;
#if X86_DEBUG
			char temp_string_offset16[12];

			sprintf(temp_string_offset16, "[%04Xh", offset_16);
			strncat(operand2_string, temp_string_offset16, 12);
#endif
		}

		strncat(operand2_string, "]", 1);

	}
	else
	{
		// completely differnet register order for fuck knows what reason
		// final offset is just a pointer so it modfies the ax reg by dereferencing it
		switch (modrm_info.rm)
		{
		case 0:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.AX;
#if X86_DEBUG
				strncat(operand2_string, "AX", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.AL;
#if X86_DEBUG
				strncat(operand2_string, "AL", 2);
#endif
			}
			break;
		case 1:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.CX;
#if X86_DEBUG
				strncat(operand2_string, "CX", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.CL;
#if X86_DEBUG
				strncat(operand2_string, "CL", 2);
#endif
			}
			break;
		case 2:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.DX;
#if X86_DEBUG
				strncat(operand2_string, "DX", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.DL;
#if X86_DEBUG
				strncat(operand2_string, "DL", 2);
#endif
			}
			break;
		case 3:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.BX;
#if X86_DEBUG
				strncat(operand2_string, "BX", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.BL;
#if X86_DEBUG
				strncat(operand2_string, "BL", 2);
#endif
			}
			break;
		case 4:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.SP;
#if X86_DEBUG
				strncat(operand2_string, "SP", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.AH;
#if X86_DEBUG
				strncat(operand2_string, "AH", 2);
#endif
			}
			break;
		case 5:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.BP;
#if X86_DEBUG
				strncat(operand2_string, "BP", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.CH;
#if X86_DEBUG
				strncat(operand2_string, "CH", 2);
#endif
			}
			break;
		case 6:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.SI;
#if X86_DEBUG
				strncat(operand2_string, "SI", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.DH;
#if X86_DEBUG
				strncat(operand2_string, "DH", 2);
#endif
			}
			break;
		case 7:
			if (w)
			{
				modrm_info.final_offset = (uint8_t*)&cpu_8086.DI;
#if X86_DEBUG
				strncat(operand2_string, "DI", 2);
#endif
			}
			else
			{
				modrm_info.final_offset = &cpu_8086.BH;
#if X86_DEBUG
				strncat(operand2_string, "BH", 2);
#endif
			}
			break;

		}
	}

#if X86_DEBUG
	//build disassembly string
	//the opcode is provided by i8086_update (for now)
	//*destination, source* syntax
	if (direction)
	{
		//destination=reg
		//source=rm
		strncat(disassembly_string, operand1_string, strlen(operand1_string));


		// group opcodes don't have rm, and just use the register - operand2 is always an immediate, and that is printed by the group opcode
		// decode code, so throw away whatever we parsed here.
		if (!i8086_IsGroupOpcode(opcode))
		{
			strncat(disassembly_string, ", ", 2);
			strncat(disassembly_string, operand2_string, strlen(operand2_string));
		}
	}
	else
	{
		//destination=rm
		//source=reg
		strncat(disassembly_string, operand2_string, strlen(operand2_string));

		// group opcodes don't have rm, and just use the register - operand2 is always an immediate, so throw away whatever we parsed here.
		if (!i8086_IsGroupOpcode(opcode))
		{
			strncat(disassembly_string, ", ", 2);
			strncat(disassembly_string, operand1_string, strlen(operand1_string));
		}

	}

	strncpy(&modrm_info.disasm, disassembly_string, DISASM_STR_SIZE);
#endif

	return modrm_info;
}

void i8086_MoveSegOff8(uint8_t value, bool direction)
{
	// modrm but for some reason both mod and reg are avoided, so it has to have its own implementation only for opcodes a0-a3. also only for the AH register.

	// default register for this is DS
	uint16_t* seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.DS * X86_PARAGRAPH_SIZE) + value];

	switch (cpu_8086.last_prefix)
	{
		case override_es:
			seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.ES * X86_PARAGRAPH_SIZE) + value];
			Logging_LogChannel("MOV AL, [ES:%02x]", value);
			break;
		case override_cs:
			seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.CS * X86_PARAGRAPH_SIZE) + value];
			Logging_LogChannel("I don't think this is a great idea (MOV AL, [CS:%02x])", LogChannel_Warning, value);
			break;
		case override_ss:
			seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.SS * X86_PARAGRAPH_SIZE) + value];
			Logging_LogChannel("MOV AL, [SS:%02x]", LogChannel_Warning, value);
			break;
	}


	if (!direction) // a0-a1
	{
		cpu_8086.AL = (uint8_t)*seg_ptr;
	}
	else // a2-a3
	{
		*seg_ptr = cpu_8086.AL;
	}

	cpu_8086.IP += 2;
}

void i8086_MoveSegOff16(uint16_t value, bool direction)
{
	// modrm but for some reason both mod and reg are avoided, so it has to have its own implementation only for opcodes a0-a3. also only for the AH register.

	// default register for this instruction is DS
	uint16_t* seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.DS * X86_PARAGRAPH_SIZE) + value];

	switch (cpu_8086.last_prefix)
	{
	case override_es:
		seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.ES * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("MOV AL, [ES:%02x]", LogChannel_Debug, value);
		break;
	case override_cs:
		seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.CS * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("I don't think this is a great idea (MOV AL, [CS:%02x])", LogChannel_Warning, value);
		break;
	case override_ss:
		seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.SS * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("MOV AL, [SS:%02x]", LogChannel_Debug, value);
		break;
	case override_ds:
	case override_none:
		seg_ptr = (uint16_t*)&cpu_8086.address_space[(cpu_8086.SS * X86_PARAGRAPH_SIZE) + value];
		Logging_LogChannel("MOV AL, [DS:%02x]", LogChannel_Debug, value);
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

	cpu_8086.IP += 3;
}

void i8086_Grp1(uint8_t opcode)
{
	uint8_t temp_imm8u_01 = i8086_ReadU8(cpu_8086._PC);
	uint8_t temp_imm8u_02 = 0x00;
	uint16_t temp_imm16u_01 = 0x00;

	cpu_8086._PC++;
	cpu_8086.IP++;

	i8086_modrm_t modrm_info = i8086_ModRM(opcode, temp_imm8u_01);

	if (opcode < 0x80 || opcode > 0x83)
	{
		Logging_LogChannel("Invalid GRP1 instruction encode %04X ext opcode from ModRM %04X, skipping [THIS IS VERY BAD, GOING OFF THE RAILS]", LogChannel_Warning, opcode, modrm_info.ext_opcode);
		cpu_8086.IP++;
		return;
	}
	// all of these are immediate bytes or word

	switch (modrm_info.ext_opcode)
	{
	case 0: // ADD
		switch (opcode)
		{
		case 0x80:
		case 0x82: 
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add8(modrm_info.reg_ptr8, &temp_imm8u_02, false);
			cpu_8086.IP += 2;
			Logging_LogChannel("ADD %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, &temp_imm16u_01, false);
			Logging_LogChannel("ADD %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, false);
			Logging_LogChannel("ADD %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 1: // OR
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Or8(modrm_info.reg_ptr8, &temp_imm8u_02);
			cpu_8086.IP += 2;
			Logging_LogChannel("OR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Or16(modrm_info.reg_ptr16, &temp_imm16u_01);
			cpu_8086.IP += 3;
			Logging_LogChannel("OR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Or16(modrm_info.reg_ptr16, (uint16_t*)temp_imm8u_02);
			cpu_8086.IP += 2;
			Logging_LogChannel("OR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;

		}
		break;
	case 2: // ADC
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add8(modrm_info.reg_ptr8, &temp_imm8u_02, true);
			cpu_8086.IP += 2;
			Logging_LogChannel("ADC %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, &temp_imm16u_01, true);
			Logging_LogChannel("ADC %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, true);
			Logging_LogChannel("ADC %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 3: // SBB
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub8(modrm_info.reg_ptr8, &temp_imm8u_02, true);
			Logging_LogChannel("SBB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, &temp_imm16u_01, true);
			Logging_LogChannel("SBB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, true);
			Logging_LogChannel("SBB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 4: // AND
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_And8(modrm_info.reg_ptr8, &temp_imm8u_02);
			Logging_LogChannel("AND %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_And16(modrm_info.reg_ptr16, &temp_imm16u_01);
			Logging_LogChannel("AND %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_And16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02);
			Logging_LogChannel("AND %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 5: // SUB
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub8(modrm_info.reg_ptr8, &temp_imm8u_02, false);
			Logging_LogChannel("SUB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, &temp_imm16u_01, false);
			Logging_LogChannel("SUB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, false);
			Logging_LogChannel("SUB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 6: // XOR
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Xor8(modrm_info.reg_ptr8, &temp_imm8u_02);
			Logging_LogChannel("XOR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Xor16(modrm_info.reg_ptr16, &temp_imm16u_01);
			Logging_LogChannel("XOR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Xor16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02);
			Logging_LogChannel("XOR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 7: // CMP
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Cmp8(modrm_info.reg_ptr8, &temp_imm8u_02);
			Logging_LogChannel("CMP %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Cmp16(modrm_info.reg_ptr16, &temp_imm16u_01);
			Logging_LogChannel("CMP %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Cmp16((uint16_t*)modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02);
			Logging_LogChannel("CMP %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	}
}

void i8086_Grp2(uint8_t opcode)
{
	uint8_t temp_imm8u_01 = i8086_ReadU8(cpu_8086._PC);
	uint8_t temp_imm8u_02 = 0x00;
	uint16_t temp_imm16u_01 = 0x00;

	cpu_8086._PC++;

	i8086_modrm_t modrm_info = i8086_ModRM(opcode, temp_imm8u_01);

	// switch operation based on ext_opcode value
	switch (modrm_info.ext_opcode)
	{
	case 0:
		switch (opcode)
		{
		case 0xD0:
			i8086_Rol8(modrm_info.reg_ptr8, 1, false);

			Logging_LogChannel("ROL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, 1, false);
			
			Logging_LogChannel("ROL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Rol8(modrm_info.reg_ptr8, cpu_8086.CL, false);
			
			Logging_LogChannel("ROL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, false);
			
			Logging_LogChannel("ROL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 1:
		switch (opcode)
		{
		case 0xD0:
			i8086_Ror8(modrm_info.reg_ptr8, 1, false);
			
			Logging_LogChannel("ROR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, 1, false);
			
			Logging_LogChannel("ROR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Ror8(modrm_info.reg_ptr8, cpu_8086.CL, false);
			
			Logging_LogChannel("ROR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, false);
			
			Logging_LogChannel("ROR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;

		}
		break;
	case 2:
		switch (opcode)
		{
		case 0xD0:
			i8086_Rol8(modrm_info.reg_ptr8, 1, true);
			
			Logging_LogChannel("RCL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, 1, true);
			
			Logging_LogChannel("RCL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Rol8(modrm_info.reg_ptr8, cpu_8086.CL, true);
			
			Logging_LogChannel("RCL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, true);
			
			Logging_LogChannel("RCL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 3:
		switch (opcode)
		{
		case 0xD0:
			i8086_Ror8(modrm_info.reg_ptr8, 1, true);
			
			Logging_LogChannel("RCR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, 1, true);
			
			Logging_LogChannel("RCR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Ror8(modrm_info.reg_ptr8, cpu_8086.CL, true);
			
			Logging_LogChannel("RCR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, true);
			
			Logging_LogChannel("RCR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;

		}
		break;
	case 4:
	case 6: // i believe SHL is the same
		switch (opcode)
		{
		case 0xD0:
			i8086_Shl8(modrm_info.reg_ptr8, 1);
			
			Logging_LogChannel("SHL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Shl16((uint16_t*)modrm_info.final_offset, 1);
			
			Logging_LogChannel("SHL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Shl8(modrm_info.reg_ptr8, cpu_8086.CL);
			
			Logging_LogChannel("SHL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Shl16((uint16_t*)modrm_info.final_offset, cpu_8086.CL);
			
			Logging_LogChannel("SHL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 5:
		switch (opcode)
		{
		case 0xD0:
			i8086_Shr8(modrm_info.reg_ptr8, 1, false);
			
			Logging_LogChannel("SHR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, 1, false);
			
			Logging_LogChannel("SHR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Shr8(modrm_info.reg_ptr8, cpu_8086.CL, false);
			
			Logging_LogChannel("SHR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, false);
			
			Logging_LogChannel("SHR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 7:
		switch (opcode)
		{
		case 0xD0:
			i8086_Shr8(modrm_info.reg_ptr8, 1, true);
			
			Logging_LogChannel("SAR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, 1, true);
			
			Logging_LogChannel("SAR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Shr8(modrm_info.reg_ptr8, cpu_8086.CL, true);
			
			Logging_LogChannel("SAR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, true);
			Logging_LogChannel("SAR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	}

	cpu_8086.IP += 2; // all of these are two byte instructions
}

void i8086_Grp3(uint8_t opcode)
{

}

void i8086_Grp4(uint8_t opcode)
{

}

void i8086_Grp5(uint8_t opcode)
{

}

