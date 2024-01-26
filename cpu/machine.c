#pragma once
#include "machine.h"
#include "8086/8086.h"
// Machine.c: Implements machine.h

binary_type		Binary_GetBinaryType();

// Gets the cpu you chose.
basecpu_t* CPU_Get()
{
	if (cmd.cpu_ver == cpu_type_i8086)
	{
		return (struct basecpu *)&cpu_8086;
	}
}
