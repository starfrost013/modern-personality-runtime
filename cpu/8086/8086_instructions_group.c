#include "8086.h"
#include "util/logging.h"
#include <string.h>

// 8086_instructions_group.c : Implements ModR/M byte decode.

void i8086_Grp1(uint8_t opcode)
{
	uint8_t temp_imm8u_01 = i8086_ReadU8(cpu_8086._PC);
	uint8_t temp_imm8u_02 = 0x00;
	uint16_t temp_imm16u_01 = 0x00;

	cpu_8086._PC++;
	cpu_8086.IP++;

	i8086_modrm_t modrm_info = i8086_ModRM(opcode, temp_imm8u_01);

	if (opcode < 0x80 || opcode > 0x83)
	{
		Logging_LogChannel("Invalid GRP1 instruction encode %04X ext opcode from ModRM %04X, skipping [THIS IS VERY BAD, GOING OFF THE RAILS]", LogChannel_Warning, opcode, modrm_info.ext_opcode);
		cpu_8086.IP++;
		return;
	}
	// all of these are immediate bytes or word

	switch (modrm_info.ext_opcode)
	{
	case 0: // ADD
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add8(modrm_info.reg_ptr8, &temp_imm8u_02, false);
			cpu_8086.IP += 2;
			Logging_LogChannel("ADD %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, &temp_imm16u_01, false);
			Logging_LogChannel("ADD %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, false);
			Logging_LogChannel("ADD %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 1: // OR
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Or8(modrm_info.reg_ptr8, &temp_imm8u_02);
			cpu_8086.IP += 2;
			Logging_LogChannel("OR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Or16(modrm_info.reg_ptr16, &temp_imm16u_01);
			cpu_8086.IP += 3;
			Logging_LogChannel("OR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Or16(modrm_info.reg_ptr16, (uint16_t*)temp_imm8u_02);
			cpu_8086.IP += 2;
			Logging_LogChannel("OR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;

		}
		break;
	case 2: // ADC
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add8(modrm_info.reg_ptr8, &temp_imm8u_02, true);
			cpu_8086.IP += 2;
			Logging_LogChannel("ADC %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, &temp_imm16u_01, true);
			Logging_LogChannel("ADC %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Add16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, true);
			Logging_LogChannel("ADC %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 3: // SBB
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub8(modrm_info.reg_ptr8, &temp_imm8u_02, true);
			Logging_LogChannel("SBB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, &temp_imm16u_01, true);
			Logging_LogChannel("SBB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, true);
			Logging_LogChannel("SBB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 4: // AND
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_And8(modrm_info.reg_ptr8, &temp_imm8u_02);
			Logging_LogChannel("AND %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_And16(modrm_info.reg_ptr16, &temp_imm16u_01);
			Logging_LogChannel("AND %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_And16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02);
			Logging_LogChannel("AND %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 5: // SUB
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub8(modrm_info.reg_ptr8, &temp_imm8u_02, false);
			Logging_LogChannel("SUB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, &temp_imm16u_01, false);
			Logging_LogChannel("SUB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Sub16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02, false);
			Logging_LogChannel("SUB %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 6: // XOR
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Xor8(modrm_info.reg_ptr8, &temp_imm8u_02);
			Logging_LogChannel("XOR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Xor16(modrm_info.reg_ptr16, &temp_imm16u_01);
			Logging_LogChannel("XOR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Xor16(modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02);
			Logging_LogChannel("XOR %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	case 7: // CMP
		switch (opcode)
		{
		case 0x80:
		case 0x82:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Cmp8(modrm_info.reg_ptr8, &temp_imm8u_02);
			Logging_LogChannel("CMP %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;
		case 0x81:
			temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			i8086_Cmp16(modrm_info.reg_ptr16, &temp_imm16u_01);
			Logging_LogChannel("CMP %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
			cpu_8086.IP += 3;
			break;
		case 0x83:
			temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);
			i8086_Cmp16((uint16_t*)modrm_info.reg_ptr16, (uint16_t*)&temp_imm8u_02);
			Logging_LogChannel("CMP %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u_02);
			cpu_8086.IP += 2;
			break;

		}
		break;
	}
}

void i8086_Grp2(uint8_t opcode)
{
	uint8_t temp_imm8u_01 = i8086_ReadU8(cpu_8086._PC);
	uint8_t temp_imm8u_02 = 0x00;
	uint16_t temp_imm16u_01 = 0x00;

	cpu_8086._PC++;

	i8086_modrm_t modrm_info = i8086_ModRM(opcode, temp_imm8u_01);

	// switch operation based on ext_opcode value
	switch (modrm_info.ext_opcode)
	{
	case 0:
		switch (opcode)
		{
		case 0xD0:
			i8086_Rol8(modrm_info.reg_ptr8, 1, false);

			Logging_LogChannel("ROL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, 1, false);

			Logging_LogChannel("ROL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Rol8(modrm_info.reg_ptr8, cpu_8086.CL, false);

			Logging_LogChannel("ROL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, false);

			Logging_LogChannel("ROL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 1:
		switch (opcode)
		{
		case 0xD0:
			i8086_Ror8(modrm_info.reg_ptr8, 1, false);

			Logging_LogChannel("ROR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, 1, false);

			Logging_LogChannel("ROR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Ror8(modrm_info.reg_ptr8, cpu_8086.CL, false);

			Logging_LogChannel("ROR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, false);

			Logging_LogChannel("ROR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;

		}
		break;
	case 2:
		switch (opcode)
		{
		case 0xD0:
			i8086_Rol8(modrm_info.reg_ptr8, 1, true);

			Logging_LogChannel("RCL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, 1, true);

			Logging_LogChannel("RCL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Rol8(modrm_info.reg_ptr8, cpu_8086.CL, true);

			Logging_LogChannel("RCL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Rol16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, true);

			Logging_LogChannel("RCL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 3:
		switch (opcode)
		{
		case 0xD0:
			i8086_Ror8(modrm_info.reg_ptr8, 1, true);

			Logging_LogChannel("RCR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, 1, true);

			Logging_LogChannel("RCR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Ror8(modrm_info.reg_ptr8, cpu_8086.CL, true);

			Logging_LogChannel("RCR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Ror16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, true);

			Logging_LogChannel("RCR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;

		}
		break;
	case 4:
	case 6: // i believe SHL is the same
		switch (opcode)
		{
		case 0xD0:
			i8086_Shl8(modrm_info.reg_ptr8, 1);

			Logging_LogChannel("SHL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Shl16((uint16_t*)modrm_info.final_offset, 1);

			Logging_LogChannel("SHL %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Shl8(modrm_info.reg_ptr8, cpu_8086.CL);

			Logging_LogChannel("SHL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Shl16((uint16_t*)modrm_info.final_offset, cpu_8086.CL);

			Logging_LogChannel("SHL %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 5:
		switch (opcode)
		{
		case 0xD0:
			i8086_Shr8(modrm_info.reg_ptr8, 1, false);

			Logging_LogChannel("SHR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, 1, false);

			Logging_LogChannel("SHR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Shr8(modrm_info.reg_ptr8, cpu_8086.CL, false);

			Logging_LogChannel("SHR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, false);

			Logging_LogChannel("SHR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	case 7:
		switch (opcode)
		{
		case 0xD0:
			i8086_Shr8(modrm_info.reg_ptr8, 1, true);

			Logging_LogChannel("SAR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD1:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, 1, true);

			Logging_LogChannel("SAR %s, 1", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD2:
			i8086_Shr8(modrm_info.reg_ptr8, cpu_8086.CL, true);

			Logging_LogChannel("SAR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		case 0xD3:
			i8086_Shr16((uint16_t*)modrm_info.final_offset, cpu_8086.CL, true);
			Logging_LogChannel("SAR %s, CL", LogChannel_Debug, modrm_info.disasm);
			break;
		}
		break;
	}

	cpu_8086.IP += 2; // all of these are two byte instructions
}

void i8086_Grp3(uint8_t opcode)
{
	uint8_t temp_imm8u_01 = i8086_ReadU8(cpu_8086._PC); // read modrm byte
	uint8_t temp_imm8u_02 = 0x00;

	cpu_8086._PC++;

	i8086_modrm_t modrm_info = i8086_ModRM(opcode, temp_imm8u_01);

	// 0xF6		GRP3A
	// 0xF7		GRP3B
	bool opcode_byte = (opcode == 0xF6);

	switch (modrm_info.ext_opcode)
	{
	case 0:
		temp_imm8u_02 = i8086_ReadU8(cpu_8086._PC);  // read modrm byte
		cpu_8086._PC++;

		if (opcode == opcode_byte)
			i8086_Test8(modrm_info.reg_ptr8, &temp_imm8u_02);
		else
			i8086_Test16((uint16_t*)modrm_info.final_offset, &temp_imm8u_02);
		Logging_LogChannel("TEST %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 1:
		// Illegal Opcode
		// But this should still do something...
		break;
	case 2:
		if (opcode == opcode_byte)
			i8086_Not8(modrm_info.reg_ptr8, &temp_imm8u_02);
		else
			i8086_Not16((uint16_t*)modrm_info.final_offset, &temp_imm8u_02);
		Logging_LogChannel("NOT %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 3:
		if (opcode == opcode_byte)
			i8086_Neg8(modrm_info.reg_ptr8, &temp_imm8u_02);
		else
			i8086_Neg16((uint16_t*)modrm_info.final_offset, &temp_imm8u_02);
	
		Logging_LogChannel("NEG %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 4:
		if (opcode == opcode_byte)
			i8086_Mul8(modrm_info.reg_ptr8);
		else
			i8086_Mul16((uint16_t*)modrm_info.final_offset);

		Logging_LogChannel("MUL %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 5:
		if (opcode == opcode_byte)
			i8086_Imul8(modrm_info.reg_ptr8);
		else
			i8086_Imul16((uint16_t*)modrm_info.final_offset);

		Logging_LogChannel("IMUL %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 6:
		if (opcode == opcode_byte)
			i8086_Div8(modrm_info.reg_ptr8);
		else
			i8086_Div16((uint16_t*)modrm_info.final_offset);

		Logging_LogChannel("DIV %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 7:
		if (opcode == opcode_byte)
			i8086_Idiv8(modrm_info.reg_ptr8);
		else
			i8086_Idiv16((uint16_t*)modrm_info.final_offset);

		Logging_LogChannel("IDIV %s", LogChannel_Debug, modrm_info.disasm);
		break;
	}
}

void i8086_Grp4(uint8_t opcode)
{
	Logging_LogChannel("GRP4 NOT IMPLEMENTED!", LogChannel_Debug);
}

void i8086_Grp5(uint8_t opcode)
{
	Logging_LogChannel("GRP5 NOT IMPLEMENTED!", LogChannel_Debug);
}

