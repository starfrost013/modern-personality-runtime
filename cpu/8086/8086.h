#pragma once
#include "..\..\core.h"
#include "..\machine.h"

//
// 8086.h
// Defines intel 8086 specific stuff, on top of basecpu.
//

#define ADDRESS_SPACE_8086		1048576

typedef struct i8086_modrm_s
{
	uint8_t mod;			//bits6-7
	uint8_t reg;			//bits3-5
	uint8_t rm;				//bits0-2
};

typedef enum i8086_prefix_s
{
	// DS by default
	override_none = 0,

	override_es = 1,
	
	override_ss = 2,

	override_cs = 3,

	override_ds = 4,

} i8086_prefix_t;

typedef enum i8086_repeat_s
{
	repeat_none = 0,

	/// <summary>
	/// Repeat until CX=0 and Zero Flag Set/Repeat until Equal (normal order)
	/// REP/REP/REPZ
	/// </summary>
	repeat_zero = 1,

	/// <summary>
	/// Repeat until CX=0 and Zero Flag Not Set/Repeat until Not Equal (reverse order?)
	/// REPNZ/REPNE
	/// </summary>
	repeat_nonzero = 2,

} i8086_repeat_t;

typedef struct i8086_s
{
	struct			basecpu;
	uint8_t			address_space[ADDRESS_SPACE_8086];

	bool			halted;				// 'HLT' instruction ran
	bool			int3;				// 'INT3' instruction ran
	bool			interrupt_waiting;	// Is there an interrupt waiting?
	i8086_prefix_t	last_prefix;		// holds the last prefix (ES:/SS:)
} i8086_t;

i8086_t cpu_8086;

void i8086_Init();
void i8086_Update();

uint8_t i8086_ReadU8(uint32_t position);
int8_t i8086_ReadS8(uint32_t position);
uint16_t i8086_ReadU16(uint32_t position);
int16_t i8086_ReadS16(uint32_t position);

// Required for segaddressing
uint32_t i8086_ReadU32(uint32_t position);
uint32_t i8086_ReadS32(uint32_t position);

// utilities for setting flags
void i8086_SetCF8(uint8_t result);																// Set carry flag based on 8-bit result.
void i8086_SetCF16(uint16_t result);															// Set carry flag based on 16-bit result.
void i8086_SetAF8(uint8_t result, uint8_t source, uint8_t destination);							// Set aux/halfcarry flag based on 8-bit result.
void i8086_SetAF16(uint16_t result, uint16_t source, uint16_t destination);						// Set aux/halfcarry flag based on 16-bit result.
void i8086_SetOF8(uint8_t result, uint8_t source, uint8_t destination, bool isSubtracting);		// Set overflow flag based on 8-bit result.
void i8086_SetOF16(uint16_t result, uint16_t source, uint16_t destination, bool isSubtracting);	// Set overflow flag based on 16-bit result.
void i8086_SetZF8(uint8_t result);																// Set zero flag based on 8-bit result.
void i8086_SetZF16(uint16_t result);															// Set zero flag based on 16-bit result.
void i8086_SetPF8(uint8_t result);																// Set parity flag based on 8-bit result.
void i8086_SetPF16(uint16_t result);															// Set parity flag based on 16-bit result.
void i8086_SetSF8(uint8_t result);																// Set sign flag based on 8-bit result.
void i8086_SetSF16(uint16_t result);															// Set sign flag based on 8-bit result.

// other flags (IF,DF,TF) are set by code.

// instructions both filtered through immediates and MOdR/m
void i8086_Add8(uint8_t* destination, uint8_t source, bool adc);	// 8-bit ADD/ADC: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Add16(uint16_t* destination, uint16_t source, bool adc);	// 16-bit ADD/ADC: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.