#pragma once
#include "..\core.h"
#include "..\dos\dos.h"
#include "..\cmd\cmd.h"

// Shared header file for intel x86 processors.
// This has stuff that is shared by every single x86-based processor.

// Value resolution of x86 segment registers
#define X86_PARAGRAPH_SIZE		16

struct basecpu
{
	// Accumulator register
	// Type punning terrorism

	// ALL OF THESE BYTES HAVE TO BE FLIPPED ON BIG-ENDIAN SYSTEMS!
	// for some reason my define doesn't work

	union
	{
		struct // use as uint16_t (bit punning)
		{
			uint8_t		AL; // (AX && 8);
			uint8_t		AH; // (AX >> 8);
		};
		uint16_t		AX;
	};

	// BX: Index register for some segment operations

	union
	{
		struct // use as uint16_t
		{
			uint8_t		BL; // (BX && 8);
			uint8_t		BH; // (BX >> 8);
		};
		uint16_t		BX;
	};

	union
	{
		// CX - Loop Register
		struct // use as uint16_t
		{
			uint8_t		CL; // (CX && 8);
			uint8_t		CH; // (CX >> 8);
		};
		uint16_t		CX; // whole 16 bits
	};

	// DX - Used for strings
	union
	{
		struct
		{
			uint8_t		DL; // (DL && 8);
			uint8_t		DH; // (DX >> 8);
		};
		uint16_t		DX;
	};

	// base pointer
	uint16_t	BP;
	// Stack pointer
	uint16_t	SP;
	uint32_t	_realSP;
	// Segment registers (note DS and ES are pretty much advisory names and programs can do anything)
	uint16_t	CS;		// code segment (also not advisory, (cs*16)+ip = program counter
	uint16_t	IP;
	uint32_t	_PC; // = (CS * 16) + IP;	// NOT A REAL REGISTER - 32-bit because 8086 is a 20-bit address space
	uint16_t	DS;		// data segment
	uint16_t	ES;		// extra segment (they were feeling nice that day)
	uint16_t	SS;		// stack segment (this IS not advisory (ss*16)+sp)
	// Index registers
	uint16_t	DI;
	uint16_t	SI;

	// bits12-14 should always be 1 FLAGS
	bool		flag_carry;				// bit0
	bool		flag_parity;			// bit2
	bool		flag_aux_carry;			// bit4
	bool		flag_zero;				// bit6
	bool		flag_sign;				// bit7 (NEGATIVE RESULT is TRUE)
	bool		flag_trap;				// bit8
	bool		flag_interrupt_enable;	// bit9
	bool		flag_direction;			// bit10
	bool		flag_overflow;			// bit11
};

// Needed for LAHF/SAHF etc
#define FLAG_CARRY				1		// Carry flag: If a carry occurred, set to 1
#define FLAG_PARITY				1 << 2	// Even parity flag (result = 1 if even number of 1s, 0 if odd)
#define FLAG_AUX_CARRY			1 << 4	// Aux carry (half-carry) flag
#define FLAG_ZERO				1 << 6  // Zero flag
#define FLAG_NEGATIVE			1 << 7  // Negative sign flag
#define FLAG_TRAP				1 << 8  // Single step flag
#define FLAG_INTERRUPT_ENABLE	1 << 9  // Interrupt enable flag
#define FLAG_DIRECTION			1 << 10  // Direction flag
#define FLAG_OVERFLOW			1 << 11  // Single step flag

bool	machine_running;		// Is the cpu actually running?

struct basecpu* CPU_Get();