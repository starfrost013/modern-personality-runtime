#pragma once

// BIOS.h: Emulates IBM PC BIOS
// Only standard IBM PC stuff is needed

// IBM PC 8253 pic hardware-generated interrupts
// SOME OF THESE MAY BE RELOCATED TO 0x50+ ON MT-DOS!
#define HW_INTERRUPT_SYSTEM_TIMER				0x08		// Hardware-reserved 286,386 (processor extension protection error), 486+ (nothing)
#define HW_INTERRUPT_KEYBOARD_DATA_READY		0x09		// IRQ1: Hardware-reserved 286,386 (processor extension protection error), 486+ (nothing)
#define HW_INTERRUPT_PC_LPT2					0x0A		// IRQ2: IBM PC - Invalid TSS 286+
#define HW_INTERRUPT_EGA_VERTICAL_RETRACE		0x0A		// EGA (some VGA) vertical retrace - Invalid TSS 286+
#define HW_INTERRUPT_IRQ2						0x0A		// Generic interrupt request 2 used for stack-switching on DOS (IRQ1 always used)
#define HW_INTERRUPT_COM2						0x0B		// IRQ3: Segment selector failed to load segment because it didn't exist 286+
#define HW_INTERRUPT_COM1						0x0C		// IRQ4: Stack fault 286+
#define HW_INTERRUPT_HDD						0x0D		// IRQ5: IBM PC, PC/XT
#define HW_INTERRUPT_LPT2						0x0D		// IRQ5: PC/XT (always LPT2)
#define HW_INTERRUPT_FLOPPY_CONTROLLER			0x0E		// IRQ6: Always floppy controller - GPF 286+
#define HW_INTERRUPT_IRQ7						0x0F		// IRQ7

#define BIOS_INTERRUPT_VIDEO					0x10		// Video Services (486+ Coprocessor Error)
#define BIOS_INTERRUPT_EQUIPMENT				0x11		// Get Equipment List
#define BIOS_INTERRUPT_GET_MEMORY				0x12		// Get Memory
#define BIOS_INTERRUPT_DISK						0x13		// Disk Services
#define BIOS_INTERRUPT_SERIAL					0x14		// Serial Services
#define BIOS_INTERRUPT_CASSETTE					0x15		// Cassette Services (basically irrelevant...return 86h for no cassette)
#define BIOS_INTERRUPT_KEYBOARD					0x16		// Keyboard Services
#define BIOS_INTERRUPT_PRINTER					0x17		// Printer Services
#define BIOS_INTERRUPT_BOOT_NO_DISK				0x18		// Diskless boot / ROM BASIC
#define BIOS_INTERRUPT_BOOT						0x19		// Boot!
#define BIOS_INTERRUPT_TIME						0x1A		// Time Services
#define BIOS_INTERRUPT_CTRL_BREAK				0x1B		// Ctrl+Break
#define BIOS_INTERRUPT_SYSTEM_TIMER_TICK		0x1C		// Tick the system timer
#define BIOS_INTERRUPT_DATA_VIDEO_PARAMETERS	0x1D		// Returns video parameter tables
#define BIOS_INTERRUPT_DATA_DISKETTE_PARAMETERS	0x1E		// Returns floppy format tables
#define BIOS_INTERRUPT_DATA_GRAPHICS_FONT		0x1F		// Returns 8x8 graphics font