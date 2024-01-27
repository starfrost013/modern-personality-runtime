#pragma once
#include "dos/dos.h"
#include <stdint.h>
#include <stdbool.h>

// BinLoader
// Loads binaries into the virtual address space for both operating systems

// Holds a binary once it has been loaded into memory.

// ****IBM PC MEMORY LAYOUT****
// 0000:0000 (00000h)				x86 ISR
// 0040:0000 (00400h)				Bios BDA
// 0050:0000 (00500h)				User memory (07c0:0000 is used for bootsector loading)
// A000:0000 (A0000h)				VRAM (CGA/MDA only used A4000+, EBDA @ 0x80000)
// C000:0000 (C0000h)				Video BIOS
// C800:0000 (C8000h)				BIOS Expansion / Option ROM
// F000:0000 (F0000h)				BIOS
//
// WE LOAD THE USER APPLICAITON AT 0050:0000

// THIS WILL MOST LIKELY CHANGE BECAUSE THE ENVIRONMENT BLOCK WILL BE PUT THEREm
#define MSDOS_LOADED_BINARY_LOCATION_SEG	0x0050 
#define MSDOS_LOADED_BINARY_LOCATION_OFF	0x0100

// when this is loaded the PSP and binary point into the CPU address space
// do these apps depend on dos memory layout?
// if mz_header is NULL this is a com file
typedef struct msdos_loaded_binary_s
{
	msdos_psp_t*	psp;						// The program segment prefix (process information structure)
	mz_header_t		mz_header;					// The mz header.
	int32_t 		code_size;					// Size of the binary (used for binary table)
	uint8_t*		binary;						// Binary (after relocations were applied to it)
} msdos_loaded_binary_t;

extern msdos_loaded_binary_t	loaded_binary_msdos;

bool COM_Load();								// Loads a DOS1.x/86DOS com file.
bool MZ_Load();									// Loads a boring DOS MZ binary (also int 21,4a handler)
bool NE_Load();									// Loads a segmented new executable file.