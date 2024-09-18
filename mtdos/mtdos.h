#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <dos.h>

// This file is part of the dev16 Runtime distribution.
// 
// mtdos.h
// Implements:	MT-DOS 0506:xxxx dynalinked calls (New Executable)
//				Signals
//				INT 21h, AH>=80h
//				Code segment insanity
//				Per-Task Data Area
//				Module Table

// Functions
bool MTDOS_Init();				// Initialise exciting DOS.

// Signals (MTDOS)
// Note that these are of **10/30/84** docs

#define MTDOS_SIGNAL_KB0		1					// User-defined key signal 0
#define MTDOS_SIGNAL_INTR		MTDOS_SIGNAL_KB0	// Ctrl+C (KB0 if not defined)
#define MTDOS_SIGNAL_KB1		2					// User-defined key signal 1
#define MTDOS_SIGNAL_KB2		3					// User-defined key signal 2
#define MTDOS_SIGNAL_MUF		4					// MUF/SM(?) signal
#define MTDOS_SIGNAL_DIVZ		5					// Division by zero
#define MTDOS_SIGNAL_OVFL		6					// INTO instruction
#define MTDOS_SIGNAL_HDERR		7					// Hard error
#define MTDOS_SIGNAL_TERM		8					// Terminate
#define MTDOS_SIGNAL_PIPE		9					// Broken pipe
#define MTDOS_SIGNAL_USER1		13					// User definition
#define MTDOS_SIGNAL_USER2		14					// User defind

// SIGABRT and SIGIPC are mentioned
// Docs inconsistent with itself...

// Signal actions
// These tell the OS what to do on the signal being received by a process (IF a handler is not set?)
#define MTDOS_SIGNAL_ACTION_TERMINATE	0			// Terminate immediately on receive
#define MTDOS_SIGNAL_ACTION_IGNORE		1			// Ignore it
#define MTDOS_SIGNAL_ACTION_ACCEPT		2			// Accept
#define MTDOS_SIGNAL_ACTION_ERROR		3			// Send error
#define MTDOS_SIGNAL_ACTION_ACKNOWLEDGE	4			// Acknowledge but do nothing?
