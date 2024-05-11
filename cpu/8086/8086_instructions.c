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