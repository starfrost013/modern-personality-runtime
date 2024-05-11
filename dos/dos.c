#include "dos.h"
#include "cmd/cmd.h"
#include "util/logging.h"
#include "binary/binary.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Dos.c: Implements dos (boring dos)

binary_type Binary_GetBinaryType()
{
	uint8_t exe_signature[2] = { 0 };

	// if we read less than 2 bytes (?!) \there's no real way this can be a valid file 
	if (fread(&exe_signature, 1, 2, cmd.handle) < 2)
	{
		Logging_LogChannel("Header type determination failed. File not long enough for a header.", LogChannel_Fatal);
		return Binary_Invalid;
	}

	// check for MZ header (Zm is also okay, as it was used in some very early compilers where they fucked up the endianness e.g. IBM Pascal 1.0)
	if ((exe_signature[0] == 'M'
		&& exe_signature[1] == 'Z')
		|| exe_signature[0] == 'Z'
		&& exe_signature[1] == 'M')
	{
		uint16_t e_lfanew;
		uint16_t num_relocs;

		Logging_LogChannel("Found MZ header.", LogChannel_Debug);

		int e_lfanew_location = 0x3C;

		// check e_lfanew to see if it's an NE. first check size
		if (fseek(cmd.handle, 0x3C, SEEK_SET) != 0)
		{
			Logging_LogChannel("File not large enough for valid MZ header.", LogChannel_Fatal);
			return Binary_Invalid;
		}

		// check if there is a relocation table and if so how many entries
		// which affects where e_lfanew would be (in practice, NEs have their own relocation table, so DOS relocation table will never be present in an NE (I hope))
		fseek(cmd.handle, 0x06, SEEK_SET);

		fread(&num_relocs, sizeof(uint16_t), 1, cmd.handle);

		if (num_relocs > 0)
		{
			// size of one reloc table entry is 4 bytes
			e_lfanew_location = 0x3C + (num_relocs * 4);
		}

		// check e_lfanew to see if it's an NE. first check size
		if (fseek(cmd.handle, e_lfanew_location, SEEK_SET) != 0)
		{
			Logging_LogChannel("?????? Invalid header. File cuts off during reloc table?", LogChannel_Fatal);
			return Binary_Invalid;
		}

		fread(&e_lfanew, sizeof(uint16_t), 1, cmd.handle);

		if (e_lfanew > 0)
		{
			// check if it's actually an NE, not a PE or something else
			uint8_t ne_header[2];

			// seek to e_lfanew
			if (fseek(cmd.handle, e_lfanew, SEEK_SET))
			{
				Logging_LogChannel("e_lfanew present but the indicated NE header location is not present. Invalid binary.", LogChannel_Fatal);
				return Binary_Invalid;
			}

			// read the header, check for NE signature
			fread(&ne_header, sizeof(uint8_t[2]), 1, cmd.handle);

			if (ne_header[0] == 'N'
				&& ne_header[1] == 'E')
			{
				Logging_LogChannel("Found New Executable header. This is a MT-DOS (or 16-bit Windows, or OS/2 1.x...) EXE", LogChannel_Debug);
				return Binary_EXE_NE;
			}
			else
			{
				Logging_LogChannel("e_lfanew valid but does not point to valid NE header. This could be an NT/Win9x PE, OS/2 LE/LX, or something else entirely.", LogChannel_Fatal);
				return Binary_Invalid;
			}
		}
		else
		{
			Logging_LogChannel("No e_lfanew. This is a regular DOS EXE", LogChannel_Debug);
			return Binary_EXE_MZ;
		}
	}
	else
	{
		Logging_LogChannel("Didn't find MZ header. Assuming COM file", LogChannel_Debug);

		// we have to assume it's a com file as they don't have a header.
		return Binary_COM;
	}
}

bool MSDOS_Init()
{
	Logging_LogChannel("Initialising MS-DOS...", LogChannel_Debug);

	Logging_LogChannel("Determining binary type for %s", LogChannel_Debug, cmd.command_line);
	binary_type loaded_binary_type = Binary_GetBinaryType();

	cmd.binary_type = loaded_binary_type;

	switch (cmd.binary_type)
	{
	case Binary_Invalid:
		return false;
	case Binary_COM:
		return COM_Load();
		break;
	case Binary_EXE_MZ:
		return MZ_Load();
		break;
	case Binary_EXE_NE:
		Logging_LogChannel("NE file - passing to MTDOS", LogChannel_Debug);
		return MTDOS_Init();

	}

	Logging_LogChannel("Tried to load invalid binary type %s", loaded_binary_type);
	return false;
}

bool MTDOS_Init()
{
	Logging_LogAll("MT-DOS not implemented!");
	return false;
}