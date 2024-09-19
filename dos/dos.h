#pragma once
#include <stdbool.h>
#include <stdint.h>

// dos.h : MS-DOS API
// Implements:
// Interrupt 20h-28h
// Interrupt 21h (DOS main API)


// Not implemented structures as of now (don't think these are all required):
// MCB: Memory Control Block
// CDS: Current Directory Structure [may need to be implemented]
// Device Driver 
// DOS3+ FCB table 
// Boring DOS4 installable filesystem driver table 
// "msdos_sysvars_v7" (Windows 9x)
// "msdos_sysvars_v8" (NTVDM)
// SHARing record (net only)
// STACKS segment
// Data segment subsegment structure
// Environment block 

#define MSDOS_PAGE_SIZE		512		// Size of one block used by the binary loader. Used to determine the size of the image of the binary being loadd.

// DON'T REMOVE THIS LINE!
// The original MS-DOS software was written in assembly language. Therefore, they didn't align their structures *AT ALL*. If you remove this,
// any structure that is a pointer into the virtual CPU address space will explode.
#pragma pack (push, 1)

// TODO:
// MT-DOS, SFT (System File Table), DPB (Drive Parameter Block), BPB (BIOS Parameter Block)

// Enumerates MS-DOS API (INT 21) levels
typedef enum dos_api_level_e
{
	// Invalid (not parsed)
	DOS_Invalid = 0,

	// MS-DOS 1.x. Completely fucking useless
	DOS_1x = 1,

	// MS-DOS 2.0 to 2.1. Substantially less useless - file handlers, drivers, IOCTL, etc
	DOS_20 = 2,

	// MS-DOS 2.11. Adds INT 21h,AH=58h (Get/set memory allocation strategy)
	DOS_211 = 3,

	// MS-DOS 2.25 - Internationalised DOS
	DOS_225 = 4,

	// MS-DOS 3.0 - Network redirection (unfinished), temp files, FLOCK, TRUENAME, more IOCTLs
	DOS_30 = 5,

	// MS-DOS 3.1 - Finished network redirection, more IOCTLs
	DOS_31 = 6,

	// MS-DOS 3.2 - Even more IOCTLs and internationalised stuff from DOS 2.25
	DOS_32 = 7,

	// MS-DOS 3.3 - More internationalisation, more handles, more good stuff
	DOS_33 = 8,

	// MS-DOS 4.0 - File commit, even more internationalisation, extended redirection, get boot drive, bugs, bloat
	DOS_40 = 9,

	// MS-DOS 5.0 - Less bloat, DOS in HIGH, ENABLE/DISABLE drive, ROM-DOS, extended versioning
	DOS_50 = 10,

	// MS-DOS 6.0 and 6.2 - Disk compression, DBLSPACE APIs, upper memory optimisation
	DOS_60 = 11,

	// MS-DOS 6.21 - no DBLSPACE
	DOS_621 = 12,

	// MS-DOS 6.22 - DRVSPACE not DBLSPACE
	DOS_622 = 13,

	// DOS7+ not implemented (just use a hypervisor...)

} dos_api_level;

// Enumerates MT-DOS (Also known as European DOS 4.0, eDOS, M/T-MSDOS, Multitasking MS-DOS 4...oh and OS/2 :^)) API levels

typedef enum mtdos_api_level_e
{
	MTDOS_Invalid = 0,

	// Build ("Internal Work #2.06") - 29 May 1984
	// the below but with multithreading (94h)
	MTDOS_Beta_5_29_84 = 1,

	// Build 4.11, 28 May 1985
	MTDOS_Beta_4_11 = 2,

	// Build 6.7, 26 November 1985
	// Preemptive multitasking, multiple screen devices, dynamic linking, process priority, session management, shared memory, semaphores, processes sharing code segments, etc etc etc etc
	MTDOS_Beta_6_7 = 3,

	// 17 November 1986 build
	// No session management, maybe no dynamic linking, hotpatching processes (aka Windows) in memory
	MTDOS_Final = 4,

	// Removed 4.1 until it's dumped
} mtdos_api_level;

// Binary type
typedef enum binary_type_e
{
	Binary_Invalid = 0,

	Binary_COM = 1,

	Binary_EXE_MZ = 2,

	Binary_EXE_NE = 3,

	// todo: Pre-NE (MTDOS May 1984), Pre-NE (WinDR5)
} binary_type;

// MS-DOS file header
// Relocatable executable

// Actual file header metadata
typedef struct mz_header_s
{
	uint16_t MZ;				// must be 'M' 'Z' (Mark Zbikowski)
	
	// MSD-OS
	uint16_t last_page_bytes;	// 0x02: Number of bytes in the last block. If 0, the image is 512-byte aligned 
	uint16_t pages;				// 0x04: Number of 512-byte blocks in the last binary.
	uint16_t num_relocs;		// 0x06: umber of entries in the file relocation table.
	uint16_t header_size;		// 0x08: Header size - IN PARAGRAPHS
	// if both minimum and maximum allocation are zero, the binary must be loaded in as high memory as possible
	uint16_t minimum_alloc;		// 0x0A: Minimum memory allocation (not always accurate) - IN PARAGRAPHS
	uint16_t maximum_alloc;		// 0x0C: Maximum memory allocation (not always accurate) - IN PARAGRAPHS
	uint16_t initial_ss;		// 0x1E: Initial stack segment
	uint16_t initial_sp;		// 0x10: Initial stack pointer (SS*16+SP = stack pointer)
	uint16_t checksum;			// 0x12: 16-bit Checksum (should be 0)
	uint16_t initial_ip;		// 0x14: Initial instruction pointer
	int16_t  initial_cs;		// 0x16: Initial code segment (CS*16+IP = instruction pointer), relative to where the program is loaded and can be negative
	uint16_t reloc_ptr;			// 0x18: Pointer to relocation
	uint16_t overlay_value;		// 0x1A: overlay value.
	uint16_t overlay_info;		// 0x1C+: Overlay info (FUCKED)

	uint16_t mtdos_bbits;		// 0x20: MT-DOS behaviour bits (PIFEDIT?), more important in betas. not sure how big these are

	uint16_t new_header;		// 0x3E: Pointer to NE

} mz_header_t;

// Relocation information entry
// This is how we load different segments of the binary into different areas of memory.
typedef struct mz_reloc_entry_s
{
	uint16_t segment;			// Segment
	uint16_t offset;			// Offset
} mz_reloc_entry_t;

// Relocation information
typedef struct mz_reloc_s
{
	uint16_t			num_entries;			// Comes from num_relocs in mz_header_t
	mz_reloc_entry_t	entries[];				// The list of entries. it goes here
} mz_reloc_t;

// MT-DOS file header
// New Executable
// (not implemented)


// MS-DOS specific structures
// Most of these are just implemented so applications can see sane defaults

// Swappable Data Area (SDA)
// MS-DOS 3.x+

// TODO: Do apps defend on the whole 1kb structure? Or just this bit (the part that is guaranteed to stay within INT21h). If true, we need to implement SDAv4
typedef struct msdos_sda_s
{
	uint8_t		printer_echo_flag;		// -0x22: DOS 3.10+: Printer echo flag (0x00=off, 0xFF=active)
	uint8_t		reserved[2];			// -0x21-0x20: Not used :(
	char		switch_char;			// -0x1F: DOS 3.30+ Switch character
	uint8_t		allocation_strategy;	// -0x1E: Allocation strategy set by INT 21 (NOT IN MT-DOS)
	uint8_t		reserved2;				// -0x1D: DOS 3.30+ reserved
	uint8_t		machine_name_count;		// -0x1C: DOS 3.30+ SHARE.EXE: Number of int21h,ax=0x5e01 calls
	char		machine_name[16];		// -0x1B: DOS 3.30+ SHARE.EXE machine name
	uint16_t	critsec_offset_0;		// -0x0B: INT 21h,ah=80 offset (patch for critical section calls)
	uint16_t	critsec_offset_1;		// -0x09: INT 21h,ah=80 offset (patch for critical section calls)
	uint16_t	critsec_offset_2;		// -0x07: INT 21h,ah=80 offset (patch for critical section calls)
	uint16_t	critsec_offset_3;		// -0x05: INT 21h,ah=80 offset (patch for critical section calls)
	uint16_t	critsec_offset_4;		// -0x03: INT 21h,ah=80 offset (patch for critical section calls)
	uint8_t		must_be_zero;			// -0x02:  hardcode to 0? might not be bug compatible
	uint8_t		reserved3;				// -0x01: padding
	// -----POINTER GOES HERE---
	uint8_t		error_mode;				// 0x00: critical error flag
	uint8_t		indos;					// 0x01: 00 if INT21 not running, FF if it is, decremented by 1 per call running
	uint8_t		crit_error_drive;		// 0x02: critical error drive, FFh if no critical erorr
	uint8_t		crit_error_locus;		// 0x03: critical error locus
	uint16_t	crit_error_code;		// 0x04: critical error code (DOS3+ extended error)
	uint8_t		crit_error_action;		// 0x06: critical error suggested action
	uint8_t		crit_error_class;		// 0x07: critical error class
	uint16_t	crit_error_ptr_di;		// 0x08: critical error pointer (DI component)
	uint16_t	crit_error_ptr_es;		// 0x0A: critical error part 
	uint16_t	dta_ptr_off;			// 0x0C: Disk Transfer Address pointer (offset)
	uint16_t	dta_ptr_seg;			// 0x0E: Disk Transfer Address pointer (segment)
	uint16_t	psp_ptr;				// 0x10: Program Segment Prefix segment address 
	uint16_t	int_23_sp;				// 0x12: Stack Pointer from INT23
	uint16_t	last_process_exit_code;	// 0x14: Last process exit code (zeroed after INT 21,AH=4D)
	uint8_t		current_drive;			// 0x16: current drive
	uint8_t		extended_break_flag;	// 0x17: extended break flag
	// determine if 0x18+ is  needed (dos internal area?)
} msdos_sda_t;

/* 
typedef struct msdos_sda_v4_s
{

} msdos_sda_v4_t;
*/

// BPB (BIOS Parameter Block)
typedef struct msdos_bpb_s
{
	uint32_t	dummy;
} msdos_bpb_t;

// DPB (Disk Parameter Block)
// maybe implemented
typedef struct msdos_dpb_s
{
	uint32_t	dummy;
} msdos_dpb_t;


// MS-DOS FCB
// File Control Block (DOS 1.x only, obsolete since 1983)
//
// DO NOT USE UNLESS YOU ARE TARGETING DOS 1.x or you are an idiot


typedef struct msdos_fcb_s
{
	uint8_t		drive_number;				// Drive number (0=A:, 1=B:...)
	char		file_name[8];				// Filename Good old 8.3! LEFT JUSTIFIED
	char		file_extension[3];			// File extension

	// 'Implementation dependent' section
	// Some of these are only in some dos versions
	uint16_t	block_number;				// Zero when the file is opened; thecurrent block number and the currentrecord number combined make up the record pointer
	uint16_t	record_size;				// Current FCB record size
	uint32_t	file_size;					// Current file size
	
	// MS-DOS date format:
	// Bits			Purpose
	// 0-4			Day of the month (1-31)
	// 5-8			Month (1=Jan, 2=Feb...)
	// 9-15			Year (offset from 1980) 
	uint16_t	date_stamp;					// Current date stamp

	// MS-DOS time format:
	// Bits			Purpose
	// 0-4			(seconds/2)
	// 5-10			minute (0-59)
	// 11-15		hour (11-15)
	uint16_t	time_stamp;					// Current time stamp (DOS1.1+ only)

	uint8_t		reserved[8];

	uint8_t		record_number_seq;			// Record number in current section for sequential access	
											// 128 records per 512-byte block. Not initialised by DOS.

	uint32_t	record_number_random;		// Record number for random access.
											// Used for INT 21,AH=21,22,27,28. if record size > 64b only first 3 bytes used
} msdos_fcb_t;

// Extended FCB
// ???


// MS-DOS System File Table Entry (MS-DOS 2.x)
// Partial implementation (no SHARE support!)
typedef struct msdos_sft_entry_v2_s
{
	uint32_t	dummy;
} msdos_sft_entry_v2_t;

// MS-DOS System File Table Entry (MS-DOS 3.0)
typedef struct msdos_sft_entry_v30_s
{
	uint32_t	dummy;
} msdos_sft_entry_v30_t;

// MS-DOS System File Table Entry (MS-DOS 3.1-3.3)
typedef struct msdos_sft_entry_v31_s
{
	uint32_t	dummy;
} msdos_sft_entry_v31_t;

// MS-DOS System File Table Entry (MS-DOS 4.0)
typedef struct msdos_sft_entry_v4_s
{
	uint32_t	dummy;
} msdos_sft_entry_v4_t;

// MS-DOS System File Table
// Global file table 
// Do we implement this?
typedef struct msdos_sft_s
{
	uint16_t	next_sft_off;				// Next SFT ptr (offset is FFFF if last) [Lffset]
	uint16_t	next_sft_seg;				// Next SFT ptr (offset is FFFF if last) [Segment]
	int			num_files;					// Number of files
	// TODO: entries
} msdos_sft_t;

#define MSDOS_PSP_SIZE						0x0100	// Size of the program segment prefix.

// MS-DOS Program Segment Prefix
// Program information structure
// *******NOT COMPATIBLE IN MT-DOS*******
typedef struct msdos_psp_s
{
	uint8_t		cpm_exit[2];				// (0xCD, 0x20) 0x00: INT 20h instruction to immediately exit in the case we jump to 0000h (Null deref protection) also for CP/M compatibility (and you can terminate just by executing RET!)
	uint16_t	last_seg_addr;				// 0x02: Last segment address allocated to program - can be used as linked list to next "program"
	// In our case it doesn't matter what we call, as long as we can catch it.
	char		reserved;					// 0x03: Reserved
	uint8_t		cpm_far[5];					// (0x9A, 0xD1, 0x5E, 0xA5, 0xE0) CALL 5 trap sequence for this emulator. Real DOS would have a kernel call to INT21 here! (Only DOS1 apis supported!)
	uint16_t	int22_off;					// 0x0A: Previous programs' INT 22 (Terminate) vector, IP -- not used here
	uint16_t	int22_seg;					// 0x0C: Previous programs' INT 22 (Terminate) vector, CS -- not used here
	
	uint16_t	int23_off;					// 0x0E: Previous programs' INT 23 (Ctrl-C) vector, IP 
	uint16_t	int23_seg;					// 0x10: Previous programs' INT 23 (Ctrl-C) vector, CS
	
	uint16_t	int24_off;					// 0x12: Previous programs' INT 24 (Fatal error) vector, IP
	uint16_t	int24_seg;					// 0x14: Previous programs' INT 24 (Fatal error) vector, CS

	uint16_t	parent_psp_seg_ptr;			// 0x16: Parent PSP seg ptr (for child processes)

	uint8_t		jft[20];					// MS-DOS 2.0+ 0x18: Job File Table. Indexes into SFT. default 0x00 0x01 0x02 0x03 0x04

	uint16_t	environment_seg_ptr;		// MS-DOS 2.0+ 0x2C: Segment address of environment

	uint16_t	last_sp;					// MS-DOS 2.0+ 0x2E: last SP on INT21 entry
	uint16_t	last_ss;					// MS-DOS 2.0+ 0x30: last SS on INT21 entry

	uint16_t	jft_count;					// MS-DOS 3.0+ 0x32: JFT size (you can move the JFT around)
	uint16_t	jft_ptr_off;				// MS-DOS 3.0+ 0x34: JFT pointer (default PSP:0018) [Offset]
	uint16_t	jft_ptr_seg;				// MS-DOS 3.0+ 0x36: JFT pointer (default PSP:0018) [Segment]

	uint16_t	previous_psp_ptr_off;		// MS-DOS 3.0+ 0x38: Default FFFF:FFFF. Previous PSP pointer for parent process. Used by SHARE in DOS 3.3.
	uint16_t	previous_psp_ptr_seg;		// MS-DOS 3.0+ 0x3A: Default FFFF:FFFF. Previous PSP pointer for parent process. Used by SHARE in DOS 3.3.
	
	uint8_t		dbcs_flag;					// MS-DOS 4.0+ 0x3B: DBCS interim console flag (set with INT 21,AX=6301h)
	uint8_t		truename_flag;				// 0x3C: APPEND truename flag.

	// rest of this is all Win3.x Process Datablock (likely Win1+ actually), OS/2 (or mdos4?), or Novell(!)...don't need it
	uint8_t		reserved2[3];
	uint8_t		fake_version;				// MS-DOS 5.0+ 0x40: Version to return on INT 21h,AH=30 (see SETVER)
	uint8_t		reserved3[16];

	uint8_t		int_21_return[3];			// (0xCD, 0x21, 0xCB) MS-DOS 2.0+ 0x50: Int 21 call and return far
	uint8_t		reserved4[9];				// last 7 can be used to make fcb1 extended

	msdos_fcb_t	fcb1;						// 0x5C: Unopened FCB 1 for this program
	msdos_fcb_t fcb2;						// 0x6C: Unopened FCB 2 for this program (if fcb1 opened, overwritten)

	uint8_t		cmdline_size;				// Size of command line
	char		cmdline[127];				// 0x80: Command-line information
	uint8_t		end;				// (0x0D): Denotes end of command-line information
} msdos_psp_t;

// MS-DOS 2.x
// SYSVARS_v2 (list of lists)
// This structure changed a lot (doesn't exist in DOS 1.x)

// The only reason we need to init this (as we don't actually use it and fake it)
// is because some programs use it anyway despite not being meant to
// (normal for DOS)
typedef struct msdos_sysvars_v2_s
{
	uint16_t	fcb_lru_caching;			// -0x16: Least Recently Used FCB cache (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	fcb_lru_open;				// -0x14: Least Recently Used FCB open (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	oem_function_handler_off;	// -0x12: OEM function handler IP
	uint16_t	oem_function_handler_seg;	// -0x10: OEM function handler CS
	uint16_t	int21_return_seg;			// -0x0E: int21 return CS
	int			reserved[12];				// not used in DOS 2 version
	uint16_t	mcb_ptr;					// -0x02: Memory Control Block (MCB) segaddr pointer. We don't use it here.
	
	// ----RETURN POINTER TO THIS ITEM---- (let's put it at 0116:????) depending on the fuck
	uint16_t	dpb_ptr_off;				// 0x00: first DPB pointer [Offset]
	uint16_t	dpb_ptr_seg;				// 0x02: first DPB pointer [Segment]
	uint16_t	sft_ptr_off;				// 0x04: first System File Table pointer [Offset]
	uint16_t	sft_ptr_seg;				// 0x06: first System File Table pointer [Segment]
	uint16_t	clock_ptr_off;				// 0x08: Active clock$ device ptr. We do not implement this. [Offset]
	uint16_t	clock_ptr_seg;				// 0x0A: Active clock$ device ptr. We do not implement this. [Segment]
	uint16_t	console_ptr_off;			// 0x0C: Active con$ device ptr. We do not implement this.
	uint16_t	console_ptr_seg;			// 0x0E: Active con$ device ptr. We do not implement this.

	// ----VERSION SPECIFIC AREA----
	uint8_t		drives_in_system;			// 0x10: drives in system
	uint16_t	bytes_per_block_device;		// 0x11: Maximum bytes per block device.
	uint16_t	disk_buf_ptr_off;			// 0x13: Disk buffer[Offset]
	uint16_t	disk_buf_ptr_seg;			// 0x15: Disk buffer[Segment]
	uint8_t		nul_header[18];				// 0x17: NUL device header.
} msdos_sysvars_v2_t;

// MS-DOS 3.0
// SYSVARS_v3 (list of lists)
typedef struct msdos_sysvars_v30_s
{
	uint16_t	fcb_lru_caching;			// -0x16: Least Recently Used FCB cache (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	fcb_lru_open;				// -0x14: Least Recently Used FCB open (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	oem_function_handler_off;	// -0x12: OEM function handler IP
	uint16_t	oem_function_handler_seg;	// -0x10: OEM function handler CS
	uint16_t	int21_return_seg;			// -0x0E: int21 return CS
	int			reserved[6];				// not used in DOS 3.0 version
	uint16_t	disk_buf_ptr_off;			// -0x08: Disk buffer ptr [Offset]
	uint16_t	disk_buf_ptr_seg;			// -0x06: Disk buffer ptr [Segment]
	uint16_t	unread_console_input;		// -0x04: Pointer to unread console input (not used)
	uint16_t	mcb_ptr;					// -0x02: Memory Control Block (MCB) segaddr pointer. We don't use it here.

	// ----RETURN POINTER TO THIS ITEM---- (let's put it at 0116:????) depending on the fuck
	uint16_t	dpb_ptr_off;				// 0x00: first DPB pointer [Offset]
	uint16_t	dpb_ptr_seg;				// 0x02: first DPB pointer [Segment]
	uint16_t	sft_ptr_off;				// 0x04: first System File Table pointer [Offset]
	uint16_t	sft_ptr_seg;				// 0x06: first System File Table pointer [Segment]
	uint16_t	clock_ptr_off;				// 0x08: Active clock$ device ptr. We do not implement this. [Offset]
	uint16_t	clock_ptr_seg;				// 0x0A: Active clock$ device ptr. We do not implement this. [Segment]
	uint16_t	console_ptr_off;			// 0x0C: Active con$ device ptr. We do not implement this.
	uint16_t	console_ptr_seg;			// 0x0E: Active con$ device ptr. We do not implement this.

	// ----VERSION SPECIFIC AREA----
	uint8_t		block_devices_in_system;	// 0x10: Number of block devices in system
	uint16_t	bytes_per_block_device;		// 0x11: Maximum bytes per block device.
	uint16_t	disk_buf_first_ptr_off;		// 0x13: First disk buffer[Offset]
	uint16_t	disk_buf_first_ptr_seg;		// 0x15: First disk buffer[Segment]
	uint16_t	cds_ptr_off;				// 0x17: Pointer to Current Directory Structure (CDS) [Offset]
	uint16_t	cds_ptr_seg;				// 0x19: Pointer to Current Directory Structure (CDS) [Segment]
	uint8_t		last_drive;					// 0x1A: Last drive (default 5)
	uint16_t	strings_off;				// 0x1C: Strings workspace area pointer (STRINGS= config.sys) [Offset]
	uint16_t	strings_seg;				// 0x1E: Strings workspace area pointer (STRINGS= config.sys) [Segment]
	uint16_t	strings_size;				// 0x20: Size of strings area (STRINGS= value from config.sys)
	uint16_t	fcb_table_ptr_off;			// 0x22: FCB Table Pointer [Offset]
	uint16_t	fcb_table_ptr_seg;			// 0x24: FCB Table Pointer [Segment]
	uint16_t	protected_fcbs;				// 0x26: Protected FCBS (y in FCB=x,y in CONFIG.SYS)
	uint8_t		nul_header[18];				// 0x28: NUL device header.
} msdos_sysvars_v30_t;

// MS-DOS 3.1-3.3
// SYSVARS_v31 (list of lists)
typedef struct msdos_sysvars_v31_s
{
	uint16_t	cx_machine_name;			// -0x16: Contents of CX from INT 21,AX=5e01 (SET MACHINE NAME)
	uint16_t	fcb_lru_open;				// -0x14: Least Recently Used FCB open (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	oem_function_handler_off;	// -0x12: OEM function handler IP
	uint16_t	oem_function_handler_seg;	// -0x10: OEM function handler CS
	uint16_t	int21_return_seg;			// -0x0E: int21 return CS
	int			reserved[6];				// not used in DOS 3.0 version
	uint16_t	disk_buf_ptr_off;			// -0x08: Disk buffer ptr [Offset]
	uint16_t	disk_buf_ptr_seg;			// -0x06: Disk buffer ptr [Segment]
	uint16_t	unread_console_input;		// -0x04: Pointer to unread console input (not used)
	uint16_t	mcb_ptr;					// -0x02: Memory Control Block (MCB) segaddr pointer. We don't use it here.

	// ----RETURN POINTER TO THIS ITEM---- (let's put it at 0116:????) depending on the fuck
	uint16_t	dpb_ptr_off;				// 0x00: first DPB pointer [Offset]
	uint16_t	dpb_ptr_seg;				// 0x02: first DPB pointer [Segment]
	uint16_t	sft_ptr_off;				// 0x04: first System File Table pointer [Offset]
	uint16_t	sft_ptr_seg;				// 0x06: first System File Table pointer [Segment]
	uint16_t	clock_ptr_off;				// 0x08: Active clock$ device ptr. We do not implement this. [Offset]
	uint16_t	clock_ptr_seg;				// 0x0A: Active clock$ device ptr. We do not implement this. [Segment]
	uint16_t	console_ptr_off;			// 0x0C: Active con$ device ptr. We do not implement this.
	uint16_t	console_ptr_seg;			// 0x0E: Active con$ device ptr. We do not implement this.

	// ----VERSION SPECIFIC AREA----
	uint16_t	bytes_per_block_device;		// 0x10: Maximum bytes per secto of block device.
	uint16_t	disk_buf_first_ptr_off;		// 0x13: First disk buffer[Offset]
	uint16_t	disk_buf_first_ptr_seg;		// 0x15: First disk buffer[Segment]
	uint16_t	cds_ptr_off;				// 0x16: Pointer to Current Directory Structure (CDS) [Offset]
	uint8_t		lastngs_size;				// 0x1A: Size of strings area (STRINGS= value from config.sys)
	uint16_t	fcb_table_ptr_off;			// 0x1A: FCB Table Pointer [Offset]
	uint16_t	fcb_table_ptr_seg;			// 0x1C: FCB Table Pointer [Segment]
	uint16_t	protected_fcbs;				// 0x1E: Protected FCBS (y in FCB=x,y in CONFIG.SYS)
	uint8_t		num_block_devices;			// 0x20: Number of block devices.
	uint8_t		num_drive_letters;			// 0x21: Number of drive letters
	uint8_t		nul_header[18];				// 0x22: NUL device header.
	
	uint8_t		num_joined_drives;			// 0x34: Number of joined drives.
} msdos_sysvars_v31_t;

// MS-DOS 4.0
// SYSVARS_v4 (list of lists)
typedef struct msdos_sysvars_v4_s
{
	uint16_t	cx_machine_name;			// -0x16: Contents of CX from INT 21,AX=5e01 (SET MACHINE NAME)
	uint16_t	fcb_lru_open;				// -0x14: Least Recently Used FCB open (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	oem_function_handler_off;	// -0x12: OEM function handler IP
	uint16_t	oem_function_handler_seg;	// -0x10: OEM function handler CS
	uint16_t	int21_return_seg;			// -0x0E: int21 return CS
	int			reserved[6];				// not used in DOS 3.0 version
	uint16_t	disk_buf_ptr_off;			// -0x08: Disk buffer ptr [Offset]
	uint16_t	disk_buf_ptr_seg;			// -0x06: Disk buffer ptr [Segment]
	uint16_t	unread_console_input;		// -0x04: Pointer to unread console input (not used)
	uint16_t	mcb_ptr;					// -0x02: Memory Control Block (MCB) segaddr pointer. We don't use it here.

	// ----RETURN POINTER TO THIS ITEM---- (let's put it at 0116:????) depending on the fuck
	uint16_t	dpb_ptr_off;				// 0x00: first DPB pointer [Offset]
	uint16_t	dpb_ptr_seg;				// 0x02: first DPB pointer [Segment]
	uint16_t	sft_ptr_off;				// 0x04: first System File Table pointer [Offset]
	uint16_t	sft_ptr_seg;				// 0x06: first System File Table pointer [Segment]
	uint16_t	clock_ptr_off;				// 0x08: Active clock$ device ptr. We do not implement this. [Offset]
	uint16_t	clock_ptr_seg;				// 0x0A: Active clock$ device ptr. We do not implement this. [Segment]
	uint16_t	console_ptr_off;			// 0x0C: Active con$ device ptr. We do not implement this.
	uint16_t	console_ptr_seg;			// 0x0E: Active con$ device ptr. We do not implement this.

	// ----VERSION SPECIFIC AREA----
	uint16_t	bytes_per_block_device;		// 0x10: Maximum bytes per secto of block device.
	uint16_t	disk_buf_info_ptr_off;		// 0x12: Disk buffer info pointer. [Offset]
	uint16_t	disk_buf_info_ptr_seg;		// 0x14: Disk buffer info segment. [Offset]
	uint16_t	cds_ptr_off;				// 0x16: Pointer to Current Directory Structure (CDS) [Offset]
	uint8_t		lastngs_size;				// 0x1A: Size of strings area (STRINGS= value from config.sys)
	uint16_t	fcb_table_ptr_off;			// 0x1A: FCB Table Pointer [Offset]
	uint16_t	fcb_table_ptr_seg;			// 0x1C: FCB Table Pointer [Segment]
	uint16_t	protected_fcbs;				// 0x1E: Protected FCBS (y in FCB=x,y in CONFIG.SYS)
	uint8_t		num_block_devices;			// 0x20: Number of block devices.
	uint8_t		num_drive_letters;			// 0x21: Number of drive letters
	uint8_t		nul_header[18];				// 0x22: NUL device header.

	uint8_t		num_joined_drives;			// 0x34: Number of joind drives.
	uint16_t	setver_patch_ptr;			// 0x35: MS-DOS 4.0 SETVER patch list segment pointer (predecessor to SETVER utility)
	uint16_t	ifs_util_ptr_off;			// 0x37: IFS util ptr [Offset]
	uint16_t	ifs_util_ptr_seg;			// 0x39: IFS util ptr [Segment]
	uint16_t	ifs_driver_ptr_off;			// 0x3B: IFS driver table ptr [Offset]
	uint16_t	ifs_driver_ptr_seg;			// 0x3D: IFS driver table ptr [Segment]

	uint16_t	num_buffers;				// 0x3F: Number of buffers
	uint16_t	num_buffers_lookahead;		// 0x41: Number of lookahead buffers

	uint8_t		boot_drive;					// 0x43: bootdrive (1=A...)
	uint8_t		i80386_dword_move_flag;		// 0x44: on 386+, use DWORD moves for memory (speed) ALWAYS ZERO ON <=286, 01h on 386+

	uint16_t	ext_memory_size;			// 0x45: Extended memory size (KB)
} msdos_sysvars_v4_t;

// MS-DOS 5.x and 6.x
// SYSVARS_v5 (list of lists)
typedef struct msdos_sysvars_v5
{
	uint16_t	cx_machine_name;			// -0x16: Contents of CX from INT 21,AX=5e01 (SET MACHINE NAME)
	uint16_t	fcb_lru_open;				// -0x14: Least Recently Used FCB open (SEE SYSTEM FILE TABLE!!!!!)
	uint16_t	oem_function_handler_off;	// -0x12: OEM function handler IP
	uint16_t	oem_function_handler_seg;	// -0x10: OEM function handler CS
	uint16_t	int21_return_seg;			// -0x0E: int21 return CS
	int			reserved[6];				// not used in DOS 3.0 version
	uint16_t	disk_buf_ptr_off;			// -0x08: Disk buffer ptr [Offset]
	uint16_t	disk_buf_ptr_seg;			// -0x06: Disk buffer ptr [Segment]
	uint16_t	unread_console_input;		// -0x04: Pointer to unread console input (not used)
	uint16_t	mcb_ptr;					// -0x02: Memory Control Block (MCB) segaddr pointer. We don't use it here.

	// ----RETURN POINTER TO THIS ITEM---- (let's put it at 0116:????) depending on the fuck
	uint16_t	dpb_ptr_off;				// 0x00: first DPB pointer [Offset]
	uint16_t	dpb_ptr_seg;				// 0x02: first DPB pointer [Segment]
	uint16_t	sft_ptr_off;				// 0x04: first System File Table pointer [Offset]
	uint16_t	sft_ptr_seg;				// 0x06: first System File Table pointer [Segment]
	uint16_t	clock_ptr_off;				// 0x08: Active clock$ device ptr. We do not implement this. [Offset]
	uint16_t	clock_ptr_seg;				// 0x0A: Active clock$ device ptr. We do not implement this. [Segment]
	uint16_t	console_ptr_off;			// 0x0C: Active con$ device ptr. We do not implement this.
	uint16_t	console_ptr_seg;			// 0x0E: Active con$ device ptr. We do not implement this.

	// ----VERSION SPECIFIC AREA----
	uint16_t	bytes_per_block_device;		// 0x10: Maximum bytes per secto of block device.
	uint16_t	disk_buf_info_ptr_off;		// 0x12: Disk buffer info pointer. [Offset]
	uint16_t	disk_buf_info_ptr_seg;		// 0x14: Disk buffer info segment. [Offset]
	uint16_t	cds_ptr_off;				// 0x16: Pointer to Current Directory Structure (CDS) [Offset]
	uint8_t		lastngs_size;				// 0x1A: Size of strings area (STRINGS= value from config.sys)
	uint16_t	fcb_table_ptr_off;			// 0x1A: FCB Table Pointer [Offset]
	uint16_t	fcb_table_ptr_seg;			// 0x1C: FCB Table Pointer [Segment]
	uint16_t	protected_fcbs;				// 0x1E: Protected FCBS (y in FCB=x,y in CONFIG.SYS)
	uint8_t		num_block_devices;			// 0x20: Number of block devices.
	uint8_t		num_drive_letters;			// 0x21: Number of drive letters
	uint8_t		nul_header[18];				// 0x22: NUL device header.

	uint8_t		num_joined_drives;			// 0x34: Number of joind drives.
	uint16_t	setver_patch_ptr;			// 0x35: Hardcoded kernel SETVER patch list segment pointer (predecessor to SETVER utility)
	uint16_t	setver_ptr_off;				// 0x37: SETVER utility version table [Offset] - 0000:0000h if no setver
	uint16_t	setver_ptr_seg;				// 0x39: SETVER utility version table [Segment]
	uint16_t	a20_fix;					// 0x3B: per RBIL "(DOS=HIGH) offset in DOS CS of function to fix A20 control when executing special.COM format"
	uint16_t	hma_psp;					// 0x3D: most-recently executed program's PSP (0000h if low) - used for counting a20 disabling int21 calls

	uint16_t	num_buffers;				// 0x3F: Number of buffers
	uint16_t	num_buffers_lookahead;		// 0x41: Number of lookahead buffers

	uint8_t		boot_drive;					// 0x43: bootdrive (1=A...)
	uint8_t		i80386_dword_move_flag;		// 0x44: on 386+, use DWORD moves for memory (speed) ALWAYS ZERO ON <=286, 01h on 386+

	uint16_t	ext_memory_size;			// 0x45: Extended memory size (KB)
} msdos_sysvars_v5_t;

// 
// MT-DOS stuff here:
// Per-Task Data Area (PTDA)
// MT (Module Table) and MTE (Module Table Entry) list
// Process-specific SFT
// mtdos_sysvars (no CDS)

// Interrupt numbers
#define MSDOS_INTERRUPT_TERMINATE			0x20
#define MSDOS_INTERRUPT_API					0x21
#define MSDOS_INTERRUPT_TERMINATE_ADDRESS	0x22
#define MSDOS_INTERRUPT_CTRL_C				0x23 // SIG_CTRLC - MTDOS4
#define MSDOS_INTERRUPT_FATAL_ERROR			0x24 // HDERR/HE_DAEM - MDOS4
#define MSDOS_INTERRUPT_RAW_HDD_READ		0x25
#define MSDOS_INTERRUPT_RAW_HDD_WRITE		0x26
#define MSDOS_INTERRUPT_TSR_OLD				0x27 // Dos1.X TSR (e.g. PRINT.COM)
#define MSDOS_INTERRUPT_IDLE				0x28
#define MSDOS_INTERRUPT_FAST_CONSOLE_OUTPUT	0x29
#define MSDOS_INTERRUPT_NETWORK				0x2A
#define MSDOS_INTERRUPT_DUMMY1				0x2B
#define MSDOS_INTERRUPT_DUMMY2				0x2C
#define MSDOS_INTERRUPT_DUMMY3				0x2D
#define MSDOS_INTERRUPT_PASS_TO_SHELL		0x2E
#define MSDOS_INTERRUPT_PRINT_TSR_PRINT		0x2F
#define MSDOS_INTERRUPT_CALL5_WEIRDNESS		0x30

//
// Methods
//

bool MSDOS_Init();				// Initialise boring DOS.

//
// API: Interrupts
//

void MSDOS_Int21();

void MSDOS_Int23();				// Ctrl+C

// API
// TODO: REDIRECTION

void MSDOS_ReadStdinEcho();		// INT 21h,AH=01h - Read and echo from stdin				^C/^BREAK CHECKED
void MSDOS_ReadStdin();			// INT 21h,AH=02h - Read from stdin							^C/^BREAK CHECKED

void MSDOS_WriteStdout();		// INT 21h,AH=06h - Write character represented by DL		^C/^BREAK NOT CHECKED
void MSDOS_ReadStdinRawEcho();	// INT 21h,AH=06h/DL=FFh - Raw read from stdin and echo		^C/^BREAK NOT CHECKED
void MSDOS_ReadStdinRaw();		// INT 21h,AH=07h - Raw read from stdin						^C/^BREAK NOT CHECKED

void MSDOS_PrintString();		// INT 21h,AH=09h - Prints a $-terminated string beginning at [DS:DX]
void MSDOS_GetVersion();		// INT 21h,AH=30h

// exiting
void MSDOS_Exit();				// INT 21h,AH=0 - exit
void MSDOS_ExitWithExitCode();	// INT 21h,AH=4Ah - Exits with an exit code.


#pragma pack (pop)