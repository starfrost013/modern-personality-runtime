// macros.h : Various useful global macros

#pragma once

// compiler-independent debug define
#if defined(_DEBUG) || !defined(NDEBUG)
#define		X86_DEBUG	1
#else
#define		X86_DEBUG	0
#endif

#define X86_VERSION			"0.1"

// used for ordering our registers
// will be 0x5E on little endian
// 0x0D on big endian
// works implemented as code but not as a macro?!
/*#define BIG_ENDIAN		(volatile uint32_t i = 0x0D15EA5E) \
							((*((uint8_t*)(&i))) != 0x5E);*/

// pull a single bit out of a numeric value
#define GET_BIT(val, x) (val & (1 << x)) & 1;

#define MAX_PATH_MODERN		260		// Not actually the path limit, just the modern-ish path limmit
#define MAX_PATH_LEGACY		11		// 8.3 filename