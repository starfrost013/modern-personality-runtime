#include "binary.h"
#include "util/logging.h"
#include "cpu/machine.h"
#include "cpu/8086/8086.h"
#include "cmd/cmd.h"
#include "dos/dos.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// binary.c: Implementation of Binary Loader

msdos_loaded_binary_t loaded_binary_msdos;

void Binary_Bootstrap(uint8_t* binary_data);

bool COM_Load()
{
	return false;
}

// This also serves as INT21,4A handler...
bool MZ_Load()
{
	mz_header_t mz_header;

	memset(&mz_header, 0x00, sizeof(mz_header_t));

	// Read the header...
	Logging_LogChannel("WE ARE LOADING AN MZ BINARY NOW!!!!\n", LogChannel_Debug);

	// start reading
	fseek(cmd.handle, 2, SEEK_SET);
	
	// we already verified the file was 0x3e bytes long earlier in validation and loading
	// so just read out the header
	fread(&mz_header.last_page_bytes, 1, 2, cmd.handle);
	fread(&mz_header.pages, 1, 2, cmd.handle);
	fread(&mz_header.num_relocs, 1, 2, cmd.handle);
	fread(&mz_header.header_size, 1, 2, cmd.handle);		// IN PARAGRAPHS!!
	fread(&mz_header.minimum_alloc, 1, 2, cmd.handle);		// IN PARAGRAPHS!!
	fread(&mz_header.maximum_alloc, 1, 2, cmd.handle);		// IN PARAGRAPHS!!
	fread(&mz_header.initial_ss, 1, 2, cmd.handle);
	fread(&mz_header.initial_sp, 1, 2, cmd.handle);
	fread(&mz_header.checksum, 1, 2, cmd.handle);
	fread(&mz_header.initial_ip, 1, 2, cmd.handle);
	fread(&mz_header.initial_cs, 1, 2, cmd.handle);
	fread(&mz_header.reloc_ptr, 1, 2, cmd.handle);
	fread(&mz_header.overlay_value, 1, 2, cmd.handle);
	fread(&mz_header.overlay_info, 1, 2, cmd.handle);

	// the bbits will likely not be present if reloc information is at 0x20 or earlier
	if (mz_header.reloc_ptr > 0x20)
	{
		fseek(cmd.handle, 0x20, SEEK_CUR);
		fread(&mz_header.mtdos_bbits, 1, 2, cmd.handle);
	}

	Logging_LogChannel("Executable information: ", LogChannel_Debug);
	Logging_LogChannel("Bytes after last page:\t\t%d", LogChannel_Debug, mz_header.last_page_bytes);
	Logging_LogChannel("Page count (1 page = 512 bytes):\t%d", LogChannel_Debug, mz_header.pages);
	Logging_LogChannel("Number of relocations:\t\t%d", LogChannel_Debug, mz_header.num_relocs);
	Logging_LogChannel("Header size:\t\t\t%d paragraphs (%d bytes)", LogChannel_Debug, mz_header.header_size, mz_header.header_size * X86_PARAGRAPH_SIZE);
	Logging_LogChannel("Minimum memory allocation:\t\t%d bytes", LogChannel_Debug, mz_header.minimum_alloc * X86_PARAGRAPH_SIZE);
	Logging_LogChannel("Maximum memory allocation:\t\t%d bytes", LogChannel_Debug, mz_header.maximum_alloc * X86_PARAGRAPH_SIZE);
	Logging_LogChannel("Initial SS:SP (stack location):\t%04X:%04X", LogChannel_Debug, mz_header.initial_ss, mz_header.initial_sp);
	Logging_LogChannel("Initial CS:IP (start of execution):\t%04X:%04X (CS relative to where program is loaded)", LogChannel_Debug, mz_header.initial_cs, mz_header.initial_ip);
	Logging_LogChannel("Relocation table location:\t\t0x%X", LogChannel_Debug, mz_header.reloc_ptr);
	Logging_LogChannel("Overlay value:\t\t\t%d (0 = none)", LogChannel_Debug, mz_header.overlay_value);
	Logging_LogChannel("Overlay info:\t\t\t%d", LogChannel_Debug, mz_header.overlay_info);

	if (mz_header.mtdos_bbits != 0x00)
	{
		Logging_LogChannel("MDOS4.0 (MT-DOS) behaviour bits: %04x", LogChannel_Debug, mz_header.mtdos_bbits);
	}

	// load binary information
	Logging_LogChannel("Loading code...\n", LogChannel_Debug);

	// figure out where the code starts and ends in the binary
	int32_t  code_start = mz_header.header_size * X86_PARAGRAPH_SIZE;
	int32_t  code_end = (mz_header.pages * MSDOS_PAGE_SIZE);

	// take into account that the last page may not be 512 bytes
	if (mz_header.last_page_bytes > 0)
	{
		code_end -= MSDOS_PAGE_SIZE;
		code_end += mz_header.last_page_bytes;
	}

	int32_t code_size = code_end - code_start;

	Logging_LogChannel("Binary is of size 0x%X bytes", LogChannel_Debug, code_size);

	// allocate memory 

	uint8_t* binary_data = calloc(1, sizeof(uint8_t) * code_size);

	if (binary_data == NULL)
	{	
		Logging_LogChannel("****FATAL ERROR - BUG**** Failed to allocate memory for binary (%d)", LogChannel_Fatal, errno);
		return false; 
	}

	if (fseek(cmd.handle, code_start, SEEK_SET))
	{
		Logging_LogChannel("Failed to seek to start of code (%d). Corrupted binary.", LogChannel_Fatal, errno);
		return false; 
	}

	if (fread(binary_data, 1, code_size, cmd.handle) < code_size)
	{
		Logging_LogChannel("Failed to load executable image (%d). Corrupted binary.", LogChannel_Fatal, errno);
		return false; 
	}

	if (mz_header.num_relocs > 0x00)
	{
		// allocate the relocation entries (they are temp, as we don't need them after they are done)
		mz_reloc_t* reloc_table = calloc(mz_header.num_relocs, sizeof(mz_reloc_t) + (sizeof(mz_reloc_entry_t)));

		if (reloc_table == NULL)
		{
			Logging_LogChannel("****FATAL ERROR - BUG**** Failed to allocate memory for relocation table (%d)", LogChannel_Fatal, errno);
			return false;
		}

		Logging_LogChannel("Binary has been loaded correctly. Relocating binary to %04x:%04x...", LogChannel_Debug, MSDOS_LOADED_BINARY_LOCATION_SEG, MSDOS_LOADED_BINARY_LOCATION_OFF);

		fseek(cmd.handle, mz_header.reloc_ptr, SEEK_SET);

		for (int32_t reloc_num = 0; reloc_num < mz_header.num_relocs; reloc_num++)
		{
			fread(&reloc_table->entries[reloc_num].offset, 1, 2, cmd.handle);
			fread(&reloc_table->entries[reloc_num].segment, 1, 2, cmd.handle);

			// calculate the address of the far call we are fixing up
			int32_t final_address = (reloc_table->entries[reloc_num].segment * X86_PARAGRAPH_SIZE) + reloc_table->entries[reloc_num].offset;

			//this part prevents overflow issues 

			reloc_table->entries[reloc_num].offset = (binary_data[final_address + 1] << 8) + (binary_data[final_address] & 0x00FF);
			reloc_table->entries[reloc_num].segment = (binary_data[final_address + 3] << 8) + (binary_data[final_address + 2] & 0x00FF);

			Logging_LogChannel("Fixing up reference to far address %04X:%04X (address relative to start of code 0x%04X)\n", LogChannel_Debug, 
				reloc_table->entries[reloc_num].segment, reloc_table->entries[reloc_num].offset, final_address);

			// actually do the relocation (this moves it to where in memory it is actually loaded)
			reloc_table->entries[reloc_num].offset += MSDOS_LOADED_BINARY_LOCATION_OFF;
			reloc_table->entries[reloc_num].segment += MSDOS_LOADED_BINARY_LOCATION_SEG;

			binary_data[final_address] = reloc_table->entries[reloc_num].offset & 0x00FF;
			binary_data[final_address + 1] = reloc_table->entries[reloc_num].offset >> 8;
			binary_data[final_address + 2] = reloc_table->entries[reloc_num].segment & 0x00FF;
			binary_data[final_address + 3] = reloc_table->entries[reloc_num].segment >> 8;

			uint16_t final_off = (binary_data[final_address+1] << 8) + (binary_data[final_address]);
			uint16_t final_seg = (binary_data[final_address+3] << 8) + (binary_data[final_address+2]);

			Logging_LogChannel("Loaded relocation entry #%d (new address %04X:%04X, relocated by %04X:%04X)\n", LogChannel_Debug, 
				reloc_num, final_seg, final_off, MSDOS_LOADED_BINARY_LOCATION_SEG, MSDOS_LOADED_BINARY_LOCATION_OFF);
		}
		
	}

	Logging_LogChannel("Generating program segment prefix at %04x:0000 (WARNING: NOT IMPLEMENTED!!!!)...", LogChannel_Debug, MSDOS_LOADED_BINARY_LOCATION_SEG);

	// tell the bootstrap function what the code size and header is
	loaded_binary_msdos.mz_header = mz_header;
	loaded_binary_msdos.code_size = code_size;

	Binary_Bootstrap(binary_data);
	return true;
}

bool NE_Load()
{
	Logging_LogChannel("NE not implemented!", LogChannel_Fatal);
	return false;
}

// Utility function that copies the binary binary_data into the CPU address space.
void Binary_Bootstrap(uint8_t* binary_data)
{
	Logging_LogChannel("Bootstrapping CPU for executing the program.", LogChannel_Debug);
	basecpu_t* basecpu = CPU_Get();

	if (cmd.cpu_ver == cpu_type_i8086)
	{
		i8086_t* i8086 = CPU_Get();
		loaded_binary_msdos.psp = &i8086->address_space[MSDOS_LOADED_BINARY_LOCATION_SEG * X86_PARAGRAPH_SIZE];
		loaded_binary_msdos.binary = &i8086->address_space[MSDOS_LOADED_BINARY_LOCATION_SEG * X86_PARAGRAPH_SIZE] + MSDOS_PSP_SIZE;

		Logging_LogChannel("Copying binary into emulated CPU address space beginning at %04x:%04x", LogChannel_Debug, MSDOS_LOADED_BINARY_LOCATION_SEG, MSDOS_PSP_SIZE);
		memcpy(loaded_binary_msdos.binary, binary_data, loaded_binary_msdos.code_size);
	}

	// bootstrap based on the binary  type
	switch (cmd.binary_type)
	{
	case Binary_COM:
		//TODO: SET TERMINATE AND CTRL-C ADDRESSES TO AFTER EXEC
		basecpu->CS = basecpu->DS = basecpu->SS = MSDOS_LOADED_BINARY_LOCATION_SEG;
		basecpu->IP = MSDOS_PSP_SIZE; // PSP 0x0000-0x00ff
		basecpu->SP = 0xFFFE; // in order to give you the maximum amount of space for your program in a COM file
		break;
	case Binary_EXE_MZ:
		// tell the cpu to go to the entry point and set up segment registers
		if (loaded_binary_msdos.mz_header.initial_cs > 0)
		{
			basecpu->CS = basecpu->DS = MSDOS_LOADED_BINARY_LOCATION_SEG;
		}
		else
		{
			basecpu->DS = MSDOS_LOADED_BINARY_LOCATION_SEG;
			basecpu->CS = MSDOS_LOADED_BINARY_LOCATION_SEG + loaded_binary_msdos.mz_header.initial_cs;
		}

		// set based on header values
	
		basecpu->SS = loaded_binary_msdos.mz_header.initial_ss;
		basecpu->SP = loaded_binary_msdos.mz_header.initial_sp;

		// TODO: ??????? why is this required it shouldn't be (take into account the PSP)
		basecpu->IP = loaded_binary_msdos.mz_header.initial_ip + MSDOS_PSP_SIZE;

		// TODO: ALMOST CERTAINLY WRONG!!! Just not sure about what the documentation says here...
		if (loaded_binary_msdos.psp->fcb1.drive_number > 0) 			basecpu->AL = 0;
		if (loaded_binary_msdos.psp->fcb2.drive_number > 0) 			basecpu->AH = 0;


		break;
	}
}