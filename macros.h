// macros.h : Various useful global macros

#pragma once

#define X86_VERSION			"0.1"

// used for ordering our registers
// will be 0x5E on little endian
// 0x0D on big endian
// works implemented as code but not as a macro?!
/*#define BIG_ENDIAN		(volatile uint32_t i = 0x0D15EA5E) \
							((*((uint8_t*)(&i))) != 0x5E);*/

// uint16_t: bit 0 = 32768, bit 1 = 16384, etc etc etc 
#define BIT(x) 1 << x;

#define MAX_PATH_MODERN		260		// Not actually the path limit, just the modern-ish path limmit
#define MAX_PATH_LEGACY		11		// 8.3 filename