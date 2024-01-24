#pragma once
#include "8086.h"
// 8086_util.c:
//
// Implements 8086.h flag utility functions. And probably some other stuff

void i8086_SetCF8(uint8_t result)
{
	struct basecpu* cpu = CPU_Get();
	cpu->flag_carry = (result & 0x100); // ff max
}

void i8086_SetCF16(uint16_t result)
{
	struct basecpu* cpu = CPU_Get();
	cpu->flag_carry = (result & 0x10000); // ffff max
}

void i8086_SetAF8(uint8_t result, uint8_t source, uint8_t destination)
{
	struct basecpu* cpu = CPU_Get();
	cpu->flag_aux_carry = (result ^ source ^ destination) & 0x10;
}

void i8086_SetAF16(uint16_t result, uint16_t source, uint16_t destination)
{
	struct basecpu* cpu = CPU_Get();
	cpu->flag_aux_carry = (result ^ source ^ destination) & 0x10;
}

void i8086_SetOF8(uint8_t result, uint8_t source, uint8_t destination, bool isSubtracting)
{
	struct basecpu* cpu = CPU_Get();
	
	if (!isSubtracting)
	{
		// check for positive overflow
		cpu->flag_overflow = (result ^ source) & (result ^ destination) & 0x80;
	}
	else
	{
		// check for negative overflow
		cpu->flag_overflow = (destination ^ source) & (destination ^ result) & 0x80;
	}
}

void i8086_SetOF16(uint16_t result, uint16_t source, uint16_t destination, bool isSubtracting)
{
	struct basecpu* cpu = CPU_Get();

	if (!isSubtracting)
	{
		// check for positive overflow
		cpu->flag_overflow = (result ^ source) & (result ^ destination) & 0x8000;
	}
	else
	{
		// check for negative overflow
		cpu->flag_overflow = (destination ^ source) & (destination ^ result) & 0x8000;
	}
}

void i8086_SetZF8(uint8_t result)
{
	struct basecpu* cpu = CPU_Get();

	// VERY HARD!
	cpu->flag_zero = (result == 0);
}

void i8086_SetZF16(uint16_t result)
{
	struct basecpu* cpu = CPU_Get();

	// VERY HARD!
	cpu->flag_zero = (result == 0);
}

void i8086_SetPF8(uint8_t result)
{
	struct basecpu* cpu = CPU_Get();

	// will mean 0=
	result ^= (result >> 4);
	result ^= (result >> 2);
	result ^= (result >> 1);

	cpu->flag_parity = (~result) & 1;
}

void i8086_SetPF16(uint16_t result)
{
	struct basecpu* cpu = CPU_Get();

	// will mean 0=
	result ^= (result >> 8);
	result ^= (result >> 4);
	result ^= (result >> 2);
	result ^= (result >> 1);

	cpu->flag_parity = (~result) & 1;
}

void i8086_SetSF8(uint16_t result)
{
	// this works because we only have an 8-bit number, basically checks if first bit is 1 or not (which is how twos complement determines negative or not)
	return (result & 0x80);
}

void i8086_SetSF16(uint16_t result)
{
	// this works because we only have an 16-bit number, basically checks if first bit is 1 or not (which is how twos complement determines negative or not)
	return (result & 0x8000);
}
