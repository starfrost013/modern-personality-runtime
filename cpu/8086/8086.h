#pragma once
#include "cpu/machine.h"

#include <stdint.h>
#include <stdbool.h>

//
// 8086.h
// Defines intel 8086 specific stuff, on top of basecpu.
//

#define ADDRESS_SPACE_8086		1048576					// Size of the Intel 8086 address space.

#if X86_DEBUG
#define DISASM_STR_SIZE			48						// Size of the disassembly string for ModR/M in debug builds.
#endif

typedef struct i8086_modrm_s
{
	bool		w;							//derived from opcode map, not decoded (word length, 0=8bit, 1=16bit)
	uint8_t		mod;						//bits6-7
	uint8_t*	reg_ptr8;					//bits3-5 for reg if w=0
	uint16_t*	reg_ptr16;					//bits3-5 for reg if w=1
	uint8_t		ext_opcode;					//bits3-5 for some opcodes
	uint8_t		rm;							//bits0-2

	uint8_t*	final_offset;				//filled in for memory-referencing instructions
	
#if X86_DEBUG
	char		disasm[DISASM_STR_SIZE];	//disassembled string
#endif
} i8086_modrm_t;

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
	struct			basecpu_s;
	uint8_t			address_space[ADDRESS_SPACE_8086];

	bool			halted;				// 'HLT' instruction ran
	bool			int3;				// 'INT3' instruction ran
	bool			interrupt_waiting;	// Is there an interrupt waiting?
	i8086_prefix_t	last_prefix;		// holds the last prefix (ES:/SS:)
} i8086_t;

extern i8086_t cpu_8086;

void i8086_Init();
void i8086_Update();

uint8_t i8086_ReadU8(uint32_t position);
int8_t i8086_ReadS8(uint32_t position);
uint16_t i8086_ReadU16(uint32_t position);
int16_t i8086_ReadS16(uint32_t position);

// utilities for setting flags
void i8086_SetCF8(uint8_t destination, uint8_t source);											// Set carry flag based on 8-bit result.
void i8086_SetCF16(uint16_t destination, uint16_t source);										// Set carry flag based on 16-bit result.
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

// utilities
bool i8086_IsGroupOpcode(uint8_t opcode);								// For debug disassembly: Determines if an opcode is a group opcode or not.

// instructions both filtered through immediates and MOdR/m

// arithmetic and compare
void i8086_Add8(uint8_t* destination, uint8_t* source, bool adc);		// 8-bit ADD/ADC: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Add16(uint16_t* destination, uint16_t* source, bool adc);	// 16-bit ADD/ADC: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Sub8(uint8_t* destination, uint8_t* source, bool sbb);		// 8-bit SUB/SBB: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Sub16(uint16_t* destination, uint16_t* source, bool sbb);	// 16-bit SUB/SBB: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Cmp8(uint8_t* destination, uint8_t* source);					// 8-bit compare: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Cmp16(uint16_t* destination, uint16_t* source);				// 16-bit compare: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Or8(uint8_t* destination, uint8_t* source);					// 8-bit OR: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Or16(uint16_t* destination, uint16_t* source);				// 16-bit OR: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_And8(uint8_t* destination, uint8_t* source);					// 8-bit AND: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_And16(uint16_t* destination, uint16_t* source);				// 16-bit AND: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Xor8(uint8_t* destination, uint8_t* source);					// 8-bit XOR: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Xor16(uint16_t* destination, uint16_t* source);				// 16-bit XOR: Destination must be pointer to one of the 16-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

// bit manip
// no, sal doesn't exist on 8086.
void i8086_Shl8(uint8_t* destination, uint8_t amount);					// 8-bit SHL: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Shl16(uint16_t* destination, uint8_t amount);				// 16-bit SHL: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Shr8(uint8_t* destination, uint8_t amount, bool sar);		// 8-bit SHL: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Shr16(uint16_t* destination, uint8_t amount, bool sar);		// 16-bit SHL: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Rol8(uint8_t* destination, uint8_t amount, bool rcl);		// 8-bit SHL: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Rol16(uint16_t* destination, uint8_t amount, bool rcl);		// 16-bit SHL: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

void i8086_Ror8(uint8_t* destination, uint8_t amount, bool rcr);		// 8-bit ROR: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.
void i8086_Ror16(uint16_t* destination, uint8_t amount, bool rcr);		// 16-bit ROR: Destination must be pointer to one of the 8-bit registers inside "basecpu" structure, or a pointer into the 8086's address space.

// instruction decode
i8086_modrm_t i8086_ModRM(uint8_t opcode, uint8_t modrm);		// Parse ModR/M byte

// group instruction decode. Some instructions use the reg as an extended opcode instead.
void i8086_Grp1(uint8_t opcode);										// 0x80..0x83
void i8086_Grp2(uint8_t opcode);										
void i8086_Grp3(uint8_t opcode);										// GRP3A+B
void i8086_Grp4(uint8_t opcode);
void i8086_Grp5(uint8_t opcode);

// loop
void i8086_Loop(uint8_t destination_offset, bool condition);			// Loop instruction

// move
void i8086_MoveSegOff8(uint8_t value, bool direction);					// modrm but for some reason both mod and reg are avoided, so it has to have its own implementation only for opcodes a0-a3. also only for the AH register.
void i8086_MoveSegOff16(uint16_t value, bool direction);				// modrm but for some reason both mod and reg are avoided, so it has to have its own implementation only for opcodes a0-a3. also only for the AX register.

// jump
void i8086_JumpConditional(uint8_t destination_offset, bool condition); // Jump conditional instruction

// stack
void i8086_Push(uint16_t value);
uint16_t i8086_Pop();
