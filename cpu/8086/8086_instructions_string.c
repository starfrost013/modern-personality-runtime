#include "8086.h"
#include "util/logging.h"

// ****** Dev16 ******
// Copyright © 2022-2024 starfrost. See licensing information in the licensing file
//
// 8086_instructions_string.c: 8086 core string instructions

// Calculates source for stirng instructions
int32_t i8086_StringCalculateSource()
{
	// Is DS overridable on 8086?
	int32_t source = (cpu_8086.DS * X86_PARAGRAPH_SIZE) + cpu_8086.SI;

	// SOURCE is prefix-overridable, NOT destination!
	switch (cpu_8086.last_prefix_segment)
	{
	case override_cs:
		source = (cpu_8086.CS * X86_PARAGRAPH_SIZE) + cpu_8086.SI;
		break;
	case override_ds:
		source = (cpu_8086.DS * X86_PARAGRAPH_SIZE) + cpu_8086.SI;
		break;
	case override_es:
		source = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.SI;
		break;
	case override_ss:
		source = (cpu_8086.SS * X86_PARAGRAPH_SIZE) + cpu_8086.SI;
		break;
	}

	return source; 
}

// movsb
void i8086_Movsb()
{
	int32_t source = i8086_StringCalculateSource();
	int32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

	cpu_8086.address_space[destination] = cpu_8086.address_space[source];

	// Direction flag handled here.
	// REP handled somewhere else

	if (cpu_8086.flag_direction)
	{
		cpu_8086.SI--;
		cpu_8086.DI--;
	}
	else
	{
		cpu_8086.SI++;
		cpu_8086.DI++;
	}
		
}

void i8086_Movsw()
{
	int32_t source = i8086_StringCalculateSource();
	int32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

	cpu_8086.address_space[destination] = cpu_8086.address_space[source];
	cpu_8086.address_space[destination + 1] = cpu_8086.address_space[source + 1];

	// Direction flag handled here.
	// REP handled somewhere else

	if (cpu_8086.flag_direction)
	{
		cpu_8086.SI -= 2;
		cpu_8086.DI -= 2;
	}
	else
	{
		cpu_8086.SI += 2;
		cpu_8086.DI += 2;
	}

}

void i8086_Cmpsb()
{

}

void i8086_Cmpsw()
{

}

void i8086_Stosb()	
{

}

void i8086_Stosw()
{

}

void i8086_Lodsb()
{

}


void i8086_Lodsw()
{

}

void i8086_Scasb()
{

}
void i8086_Scasw()
{

}
