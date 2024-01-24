#pragma once
#include "8086.h"

extern i8086_t cpu_8086;

void i8086_Init()
{
	Logging_LogChannel("The CPU is coming up.", LogChannel_Debug);
	// TODO: Set CPU power-on state...
}

void i8086_Update()
{
	Logging_LogChannel("Successfully executing program.", LogChannel_Debug);

	// if we are doing this
	while (!cpu_8086.halted)
	{
		// set fake PC and real SP registers (this simplifies code later down the lie
		cpu_8086._PC = (cpu_8086.CS * X86_PARAGRAPH_SIZE) + cpu_8086.IP;
		cpu_8086._realSP = (cpu_8086.SS * X86_PARAGRAPH_SIZE) + cpu_8086.SP;
		cpu_8086.last_prefix = override_none;

		// TODO; change this
		cpu_8086.int3 = cpu_8086.flag_trap; 

#if _DEBUG
		if (cpu_8086.int3)
		{
			Logging_LogChannel("INT3!", LogChannel_Debug);
#ifdef _MSC_VER
			__debugbreak();
#else
			__builtin_trap();
#endif
			cpu_8086.int3 = false;
		}
#endif


		// read the next opcode
		uint8_t next_opcode = i8086_ReadU8(cpu_8086._PC);

		bool increment = false;

		// handle prefixes etc
		switch (next_opcode)
		{
		case 0x26:
			cpu_8086.last_prefix = override_es;
			increment = true;
			break;
		case 0x36:
			cpu_8086.last_prefix = override_ss;
			increment = true;
			break;
		}

		//TODO: REPEAT PARSING

		if (increment)
		{
			cpu_8086.IP++;
			next_opcode = i8086_ReadU8(cpu_8086._PC);
		}

		uint16_t		interrupt_num = 0x00;
		uint8_t			read_imm8 = 0x00;
		uint16_t		read_imm16_01 = 0x00;
		uint16_t		read_imm16_02 = 0x00;

		switch (next_opcode)
		{
			case 0x04:
				read_imm8 = i8086_ReadU8(cpu_8086.IP++);
				i8086_Add8(&cpu_8086.AL, read_imm8, false);
				Logging_LogChannel("ADD AL, %02x", LogChannel_Debug);
				cpu_8086.IP++;
				break;
			case 0x05:
				read_imm16_01 = i8086_ReadU16(cpu_8086.IP++);

				i8086_Add16(&cpu_8086.AX, read_imm16_01, false);
				Logging_LogChannel("ADD AL, %02x", LogChannel_Debug);
			
				// we read a 16bit value
				cpu_8086.IP += 2;
				break;
			case 0x06:
				Logging_LogChannel("PUSH ES", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.ES & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.ES >> 8;
				cpu_8086.IP++;
				break;
			case 0x07:
				Logging_LogChannel("POP ES", LogChannel_Debug);
				cpu_8086.ES = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2; 
				cpu_8086.IP++;
				break;
			case 0x0E:
				Logging_LogChannel("PUSH CS", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.CS & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.CS >> 8;
				cpu_8086.IP++;
				break;
			case 0x0F:
				Logging_LogChannel("POP CS (Something has gone wrong...)", LogChannel_Warning);
				cpu_8086.CS = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x14:
				read_imm8 = i8086_ReadU8(cpu_8086.IP++);
				i8086_Add8(&cpu_8086.AL, read_imm8, true);
				Logging_LogChannel("ADC AL, %02x", LogChannel_Debug);
				cpu_8086.IP++;
				break;
			case 0x15:
				read_imm16_01 = i8086_ReadU16(cpu_8086.IP++);

				i8086_Add16(&cpu_8086.AX, read_imm16_01, true);
				Logging_LogChannel("ADC AL, %02x", LogChannel_Debug);

				// we read a 16bit value
				cpu_8086.IP += 2;
				break;
			case 0x16:
				Logging_LogChannel("PUSH SS", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.SS & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.SS >> 8;
				cpu_8086.IP++;
				break;
			case 0x17:
				Logging_LogChannel("POP SS", LogChannel_Debug);
				cpu_8086.SS = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x1E:
				Logging_LogChannel("PUSH DS", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.DS & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.DS >> 8;
				cpu_8086.IP++;
				break;
			case 0x1F:
				Logging_LogChannel("POP DS", LogChannel_Debug);
				cpu_8086.DS = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x40:
				Logging_LogChannel("INC AX", LogChannel_Debug);
				cpu_8086.AX++;
				cpu_8086.IP++;
				break;
			case 0x41:
				Logging_LogChannel("INC BX", LogChannel_Debug);
				cpu_8086.BX++;
				cpu_8086.IP++;
				break;
			case 0x42:
				Logging_LogChannel("INC CX", LogChannel_Debug);
				cpu_8086.CX++;
				cpu_8086.IP++;
				break;
			case 0x43:
				Logging_LogChannel("INC DX", LogChannel_Debug);
				cpu_8086.DX++;
				cpu_8086.IP++;
				break;
			case 0x44:
				Logging_LogChannel("INC SP", LogChannel_Debug);
				cpu_8086.SP++;
				cpu_8086.IP++;
				break;
			case 0x45:
				Logging_LogChannel("INC BP", LogChannel_Debug);
				cpu_8086.BP++;
				cpu_8086.IP++;
				break;
			case 0x46:
				Logging_LogChannel("INC SI", LogChannel_Debug);
				cpu_8086.SI++;
				cpu_8086.IP++;
				break;
			case 0x47:
				Logging_LogChannel("INC DI", LogChannel_Debug);
				cpu_8086.DI++;
				cpu_8086.IP++;
				break;
			case 0x48:
				Logging_LogChannel("DEC AX", LogChannel_Debug);
				cpu_8086.AX--;
				cpu_8086.IP++;
				break;
			case 0x49:
				Logging_LogChannel("DEC BX", LogChannel_Debug);
				cpu_8086.BX--;
				cpu_8086.IP++;
				break;
			case 0x4A:
				Logging_LogChannel("DEC CX", LogChannel_Debug);
				cpu_8086.CX--;
				cpu_8086.IP++;
				break;
			case 0x4B:
				Logging_LogChannel("DEC DX", LogChannel_Debug);
				cpu_8086.DX--;
				cpu_8086.IP++;
				break;
			case 0x4C:
				Logging_LogChannel("DEC SP", LogChannel_Debug);
				cpu_8086.SP--;
				cpu_8086.IP++;
				break;
			case 0x4D:
				Logging_LogChannel("DEC BP", LogChannel_Debug);
				cpu_8086.BP--;
				cpu_8086.IP++;
				break;
			case 0x4E:
				Logging_LogChannel("DEC SI", LogChannel_Debug);
				cpu_8086.SI--;
				cpu_8086.IP++;
				break;
			case 0x4F:
				Logging_LogChannel("DEC DI", LogChannel_Debug);
				cpu_8086.DI--;
				cpu_8086.IP++;
				break;
			case 0x50:
				Logging_LogChannel("PUSH AX", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.AX & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.AX >> 8;
				cpu_8086.IP++;
				break;
			case 0x51:
				Logging_LogChannel("PUSH CX", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.CX & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.CX >> 8;
				cpu_8086.IP++;
				break;
			case 0x52:
				Logging_LogChannel("PUSH DX", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.DX & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.DX >> 8;
				cpu_8086.IP++;
				break;
			case 0x53:
				Logging_LogChannel("PUSH BX", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.BX & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.BX >> 8;
				cpu_8086.IP++;
				break;
			case 0x54:
				Logging_LogChannel("PUSH SP", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.SP & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.SP >> 8;
				cpu_8086.IP++;
				break;
			case 0x55:
				Logging_LogChannel("PUSH BP", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.BP & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.BP >> 8;
				cpu_8086.IP++;
				break;
			case 0x56:
				Logging_LogChannel("PUSH SI", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.SI & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.SI >> 8;
				cpu_8086.IP++;
				break;
			case 0x57:
				Logging_LogChannel("PUSH DI", LogChannel_Debug);
				cpu_8086.SP -= 2;
				cpu_8086.address_space[cpu_8086._realSP] = cpu_8086.DI & 0x00FF;
				cpu_8086.address_space[cpu_8086._realSP + 1] = cpu_8086.DI >> 8;
				cpu_8086.IP++;
				break;
			case 0x58:
				Logging_LogChannel("POP AX", LogChannel_Debug);
				cpu_8086.AX  = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x59:
				Logging_LogChannel("POP CX", LogChannel_Debug);
				cpu_8086.CX = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x5A:
				Logging_LogChannel("POP DX", LogChannel_Debug);
				cpu_8086.DX = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x5B:
				Logging_LogChannel("POP BX", LogChannel_Debug);
				cpu_8086.BX = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x5C:
				Logging_LogChannel("POP SP", LogChannel_Debug);
				cpu_8086.SP = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x5D:
				Logging_LogChannel("POP BP", LogChannel_Debug);
				cpu_8086.BP = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x5E:
				Logging_LogChannel("POP SI", LogChannel_Debug);
				cpu_8086.SI = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0x5F:
				Logging_LogChannel("POP DI", LogChannel_Debug);
				cpu_8086.DI = cpu_8086.address_space[cpu_8086._realSP + 1] >> 8 + cpu_8086.address_space[cpu_8086._realSP];
				cpu_8086.SP += 2;
				cpu_8086.IP++;
				break;
			case 0xCC:
				cpu_8086.int3 = true;
				cpu_8086.IP++;
				break;
			case 0xCD:
				interrupt_num = i8086_ReadU8(cpu_8086.IP++); //increment IP and read 

				Logging_LogChannel("The application called an interrupt service routine.\n\n" 
					"INT %02Xh AX=%04X BX=%04X CX=%04X DX=%04X\n"
					"CS=%04X IP=%04X (Physical address of PC=%05X) DS=%04X ES=%04X SS=%04X\n"
					"BP=%04X DI=%04X SI=%04X", LogChannel_Debug, interrupt_num,
					cpu_8086.AX, cpu_8086.BX, cpu_8086.CX, cpu_8086.DX, cpu_8086.CS, cpu_8086.DS, cpu_8086.ES, cpu_8086.SS,
					cpu_8086.CS, cpu_8086.IP, cpu_8086._PC, cpu_8086.DS, cpu_8086.ES, cpu_8086.SS, cpu_8086.BP, cpu_8086.DI, cpu_8086.ES);
				cpu_8086.IP++;
				break;
			case 0xF5: 
				Logging_LogChannel("CMC", LogChannel_Debug);
				// IGNORE THIS WARNING, THIS IS WHAT THE INSTRUCTION IS MEANT TO DO 
				cpu_8086.flag_carry = ~cpu_8086.flag_carry; // get complement
				cpu_8086.IP++;
				break;
			case 0xF8:
				Logging_LogChannel("CLC", LogChannel_Debug);
				cpu_8086.flag_carry = false;
				cpu_8086.IP++;
				break; 
			case 0xF9:
				Logging_LogChannel("STC", LogChannel_Debug);
				cpu_8086.flag_carry = true;
				cpu_8086.IP++;
				break;
			case 0xFA:
				Logging_LogChannel("CLI", LogChannel_Debug);
				cpu_8086.flag_interrupt_enable = false;
				cpu_8086.IP++;
				break;
			case 0xFB:
				Logging_LogChannel("STI", LogChannel_Debug);
				cpu_8086.flag_interrupt_enable = true;
				cpu_8086.IP++;
				break;
			default:
				Logging_LogChannel("Unimplemented 8086 opcode 0x%X @ %04X:%04X", LogChannel_Error, next_opcode, cpu_8086.CS, cpu_8086.IP);
				cpu_8086.IP++;
				break;

		}

	}

	//todo: put in loop that keeps progrma running
	if (cpu_8086.halted
		&& cpu_8086.interrupt_waiting
		&& cpu_8086.flag_interrupt_enable)
	{
		// WAIT FOR THE INTERRUPT TO BE HANDLED AND SET INTERRUPT_WAITING TO TRUE? 
		// THIS IS NOT IMPLEMENTED!
		cpu_8086.halted = false;
	}


}

uint8_t i8086_ReadU8(uint32_t position)
{
	return cpu_8086.address_space[position];
}

int8_t i8086_ReadS8(uint32_t position)
{
	return (int8_t)cpu_8086.address_space[position];
}

uint16_t i8086_ReadU16(uint32_t position)
{
	return (cpu_8086.address_space[position + 1] << 8)
		+ (cpu_8086.address_space[position]);
}

int16_t i8086_ReadS16(uint32_t position)
{
	return (cpu_8086.address_space[position + 1] << 8)
		+ (cpu_8086.address_space[position]);
}

uint32_t i8086_ReadU32(uint32_t position)
{
	return (cpu_8086.address_space[position + 3] << 24)
		+ (cpu_8086.address_space[position + 2] << 16)
		+ (cpu_8086.address_space[position + 1] << 8)
		+ (cpu_8086.address_space[position]);
}

int32_t i8086_ReadS32(uint32_t position)
{
	return (cpu_8086.address_space[position + 3] << 24)
		+ (cpu_8086.address_space[position + 2] << 16)
		+ (cpu_8086.address_space[position + 1] << 8)
		+ (cpu_8086.address_space[position]);
}

void i8086_Add8(uint8_t* destination, uint8_t source, bool adc)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the addition
	*destination = source + *destination;
	
	// save the final value
	uint8_t final_value = *destination;

	i8086_SetOF8(final_value, source, original_value, false); // we are not subtracting
	i8086_SetCF8(final_value);
	i8086_SetAF8(final_value, source, original_value);
	i8086_SetZF8(final_value);
	i8086_SetPF8(final_value);
	i8086_SetSF8(final_value);

	if (adc)
	{
		if (cpu_8086.flag_carry)
		{
			// increment the destination value
			*destination++;
		}
	}
}


void i8086_Add16(uint16_t* destination, uint16_t source, bool adc)
{
	uint8_t original_value = 0;
	// so it doesn't get overwritten
	memcpy(&original_value, destination, sizeof(uint8_t));

	// do the addition
	*destination = source + *destination;

	// save the final value
	uint8_t final_value = *destination;

	i8086_SetOF16(final_value, source, original_value, false); // we are not subtracting
	i8086_SetCF16(final_value);
	i8086_SetAF16(final_value, source, original_value);
	i8086_SetZF16(final_value);
	i8086_SetPF16(final_value);
	i8086_SetSF16(final_value);

	if (adc)
	{
		if (cpu_8086.flag_carry)
		{
			// increment the destination value
			*destination++;
		}
	}
}