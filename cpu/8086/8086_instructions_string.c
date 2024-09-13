#include "8086.h"
#include "util/logging.h"

// ****** Dev16 ******
// Copyright © 2022-2024 starfrost. See licensing information in the licensing file
//
// 8086_instructions_string.c: 8086 core string instructions

// Calculates source for stirng instructions
uint32_t i8086_StringCalculateSource()
{
	// Is DS overridable on 8086?
	uint32_t source = (cpu_8086.DS * X86_PARAGRAPH_SIZE) + cpu_8086.SI;

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
	uint32_t source = i8086_StringCalculateSource();
	uint32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

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
		
	cpu_8086.IP++;

	Logging_LogChannel("MOVSB", LogChannel_Debug);
}

void i8086_Movsw()
{
	uint32_t source = i8086_StringCalculateSource();
	uint32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

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

	cpu_8086.IP++;
	Logging_LogChannel("MOVSW", LogChannel_Debug);
}

// compare string byte
void i8086_Cmpsb()
{
	uint32_t source = i8086_StringCalculateSource();
	uint32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

	uint32_t temporary = (source - destination);

	// set flags
	i8086_SetCF8((uint8_t)temporary);
	//todo: int32_t
	i8086_SetAF8((uint8_t)temporary, source, destination);
	i8086_SetOF8((uint8_t)temporary, source, destination, false);
	i8086_SetSF8((uint8_t)temporary);
	i8086_SetZF8((uint8_t)temporary);
	i8086_SetPF8((uint8_t)temporary);

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

	cpu_8086.IP++;
	Logging_LogChannel("cmpsb", LogChannel_Debug);
}

void i8086_Cmpsw()
{
	uint32_t source = i8086_StringCalculateSource(); 
	uint32_t destination_location = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;
	uint32_t destination = cpu_8086.address_space[destination_location] << 8 + cpu_8086.address_space[destination_location + 1];

	uint32_t temporary = (source - destination);

	// set flags
	i8086_SetCF16((uint16_t)temporary);
	//todo: int32_t
	i8086_SetAF16((uint16_t)temporary, source, destination);
	i8086_SetOF16((uint16_t)temporary, source, destination, false);
	i8086_SetSF16((uint16_t)temporary);
	i8086_SetZF16((uint16_t)temporary);
	i8086_SetPF16((uint16_t)temporary);


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

	cpu_8086.IP++;
	Logging_LogChannel("cmpsw", LogChannel_Debug);
}

void i8086_Stosb()	
{
	uint32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

	cpu_8086.address_space[destination] = cpu_8086.AL;

	// Direction flag handled here.
	// REP handled somewhere else

	// only di moved by this

	if (cpu_8086.flag_direction)
		cpu_8086.DI--;
	else
		cpu_8086.DI++;

	cpu_8086.IP++;
	Logging_LogChannel("stosb", LogChannel_Debug);
}

void i8086_Stosw()
{
	uint32_t destination = (cpu_8086.ES * X86_PARAGRAPH_SIZE) + cpu_8086.DI;

	//16bit transfer...todo: optimise?
	cpu_8086.address_space[destination] = cpu_8086.AH;
	cpu_8086.address_space[destination + 1] = cpu_8086.AL;

	// Direction flag handled here.
	// REP handled somewhere else
	// only di incremented by this instruction

	if (cpu_8086.flag_direction)
		cpu_8086.DI -= 2;
	else
		cpu_8086.DI += 2;

	cpu_8086.IP++;
	Logging_LogChannel("stosw", LogChannel_Debug);
}

void i8086_Lodsb()
{
	uint32_t source = i8086_StringCalculateSource();

	cpu_8086.AL = cpu_8086.address_space[source];

	if (cpu_8086.flag_direction)
		cpu_8086.SI--;
	else
		cpu_8086.SI++;

	cpu_8086.IP++;

	Logging_LogChannel("lodsb", LogChannel_Debug);
}

void i8086_Lodsw()
{
	uint32_t source = i8086_StringCalculateSource();

	// this will load AX because AX is a union of AL and AH
	cpu_8086.AL = cpu_8086.address_space[source];
	cpu_8086.AX = cpu_8086.address_space[source + 1];

	if (cpu_8086.flag_direction)
		cpu_8086.SI -= 2;
	else
		cpu_8086.SI -= 2;

	cpu_8086.IP++;

	Logging_LogChannel("lodsw", LogChannel_Debug);
}

void i8086_Scasb()
{
	uint32_t source = i8086_StringCalculateSource();

	uint8_t temporary = cpu_8086.AL - cpu_8086.address_space[source];

	// set flags
	i8086_SetCF16((uint8_t)temporary);
	//todo: int32_t
	i8086_SetAF16((uint8_t)temporary, source, cpu_8086.AL);
	i8086_SetOF16((uint8_t)temporary, source, cpu_8086.AL, false);
	i8086_SetSF16((uint8_t)temporary);
	i8086_SetZF16((uint8_t)temporary);
	i8086_SetPF16((uint8_t)temporary);

	if (cpu_8086.flag_direction)
		cpu_8086.DI--;
	else
		cpu_8086.DI++;

	cpu_8086.IP++;
	Logging_LogChannel("scasb", LogChannel_Debug);
}

void i8086_Scasw()
{
	uint32_t source = i8086_StringCalculateSource();

	uint16_t temporary = cpu_8086.AL - cpu_8086.address_space[source];

	// set flags
	i8086_SetCF16((uint16_t)temporary);
	//todo: int32_t
	i8086_SetAF16((uint16_t)temporary, source, cpu_8086.AX);
	i8086_SetOF16((uint16_t)temporary, source, cpu_8086.AX, false);
	i8086_SetSF16((uint16_t)temporary);
	i8086_SetZF16((uint16_t)temporary);
	i8086_SetPF16((uint16_t)temporary);

	if (cpu_8086.flag_direction)
		cpu_8086.DI -= 2;
	else
		cpu_8086.DI += 2;

	cpu_8086.IP++;

	Logging_LogChannel("scasw", LogChannel_Debug);
}
