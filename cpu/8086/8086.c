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

	while (machine_running)
	{
		// if we are doing this
		while (!cpu_8086.halted)
		{
			// set fake PC and real SP registers (this simplifies code later down the line)
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
			uint8_t next_opcode = i8086_ReadU8(cpu_8086._PC++);
			cpu_8086.IP++;

			bool increment = false;

			// handle prefixes etc
			switch (next_opcode)
			{
			case 0x26:
				cpu_8086.last_prefix = override_es;
				increment = true;
				break;
			case 0x2E:
				cpu_8086.last_prefix = override_cs;
				increment = true;
				break;
			case 0x36:
				cpu_8086.last_prefix = override_ss;
				increment = true;
				break;
			case 0x3E:
				cpu_8086.last_prefix = override_ds;
				break;
			}

			//TODO: REPEAT PARSING

			if (increment)
			{
				cpu_8086.IP++;
				cpu_8086._PC++;
				next_opcode = i8086_ReadU8(cpu_8086._PC);
			}

			uint16_t		interrupt_num = 0x00;
			uint8_t			read_imm8u = 0x00;
			int8_t			read_imm8s = 0x00;
			int16_t			read_imm16s_01 = 0x00;
			int16_t			read_imm16s_02 = 0x00;
			uint16_t		read_imm16u_01 = 0x00;
			uint16_t		read_imm16u_02 = 0x00;

			switch (next_opcode)
			{
			case 0x04:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				i8086_Add8(&cpu_8086.AL, read_imm8u, false);
				Logging_LogChannel("ADD AL, %02x", LogChannel_Debug);

				cpu_8086.IP++;
				break;
			case 0x05:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);

				i8086_Add16(&cpu_8086.AX, read_imm16u_01, false);
				Logging_LogChannel("ADD AL, %02x", LogChannel_Debug);

				// we read a 16bit value
				cpu_8086.IP++;
				break;
			case 0x06:
				Logging_LogChannel("PUSH ES", LogChannel_Debug);
				i8086_Push(cpu_8086.ES);
				cpu_8086.IP++;
				break;
			case 0x07:
				Logging_LogChannel("POP ES", LogChannel_Debug);
				cpu_8086.ES = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x0E:
				Logging_LogChannel("PUSH CS", LogChannel_Debug);
				i8086_Push(cpu_8086.CS);
				cpu_8086.IP++;
				break;
			case 0x0F:
				Logging_LogChannel("POP CS (Something has gone wrong...)", LogChannel_Warning);
				cpu_8086.CS = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x14:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				i8086_Add8(&cpu_8086.AL, read_imm8u, true);
				Logging_LogChannel("ADC AL, %02x", LogChannel_Debug);
				cpu_8086.IP++;
				break;
			case 0x15:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);

				i8086_Add16(&cpu_8086.AX, read_imm16u_01, true);
				Logging_LogChannel("ADC AL, %02x", LogChannel_Debug);

				// INCREMENT by instruction length
				cpu_8086.IP += 2;
				break;
			case 0x16:
				Logging_LogChannel("PUSH SS", LogChannel_Debug);
				i8086_Push(cpu_8086.SS);
				cpu_8086.IP++;
				break;
			case 0x17:
				Logging_LogChannel("POP SS", LogChannel_Debug);
				cpu_8086.SS = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x1E:
				Logging_LogChannel("PUSH DS", LogChannel_Debug);
				i8086_Push(cpu_8086.ES);
				cpu_8086.IP++;
				break;
			case 0x1F:
				Logging_LogChannel("POP DS", LogChannel_Debug);
				cpu_8086.DS = i8086_Pop();
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
				i8086_Push(cpu_8086.AX);
				cpu_8086.IP++;
				break;
			case 0x51:
				Logging_LogChannel("PUSH CX", LogChannel_Debug);
				i8086_Push(cpu_8086.CX);
				cpu_8086.IP++;
				break;
			case 0x52:
				Logging_LogChannel("PUSH DX", LogChannel_Debug);
				i8086_Push(cpu_8086.DX);
				cpu_8086.IP++;
				break;
			case 0x53:
				Logging_LogChannel("PUSH BX", LogChannel_Debug);
				i8086_Push(cpu_8086.BX);
				cpu_8086.IP++;
				break;
			case 0x54:
				Logging_LogChannel("PUSH SP", LogChannel_Debug);
				i8086_Push(cpu_8086.SP);
				cpu_8086.IP++;
				break;
			case 0x55:
				Logging_LogChannel("PUSH BP", LogChannel_Debug);
				i8086_Push(cpu_8086.BP);
				cpu_8086.IP++;
				break;
			case 0x56:
				Logging_LogChannel("PUSH SI", LogChannel_Debug);
				i8086_Push(cpu_8086.SI);
				cpu_8086.IP++;
				break;
			case 0x57:
				Logging_LogChannel("PUSH DI", LogChannel_Debug);
				i8086_Push(cpu_8086.DI);
				cpu_8086.IP++;
				break;
			case 0x58:
				Logging_LogChannel("POP AX", LogChannel_Debug);
				cpu_8086.AX = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x59:
				Logging_LogChannel("POP CX", LogChannel_Debug);
				cpu_8086.CX = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x5A:
				Logging_LogChannel("POP DX", LogChannel_Debug);
				cpu_8086.DX = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x5B:
				Logging_LogChannel("POP BX", LogChannel_Debug);
				cpu_8086.BX = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x5C:
				Logging_LogChannel("POP SP", LogChannel_Debug);
				cpu_8086.SP = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x5D:
				Logging_LogChannel("POP BP", LogChannel_Debug);
				cpu_8086.BP = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x5E:
				Logging_LogChannel("POP SI", LogChannel_Debug);
				cpu_8086.SI = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x5F:
				Logging_LogChannel("POP DI", LogChannel_Debug);
				cpu_8086.DI = i8086_Pop();
				cpu_8086.IP++;
				break;
			case 0x70:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_overflow);

				(read_imm8s < 0) ? Logging_LogChannel("JO -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JO +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x71:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, !cpu_8086.flag_overflow);

				// print the direction we want to change
				(read_imm8s < 0) ? Logging_LogChannel("JNO -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JNO +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x72:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_carry);

				(read_imm8s < 0) ? Logging_LogChannel("JB -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JB +%02Xh", LogChannel_Debug, read_imm8s);

				break;
			case 0x73:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, !cpu_8086.flag_carry);

				(read_imm8s < 0) ? Logging_LogChannel("JAE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JAE +%02Xh", LogChannel_Debug, read_imm8s);

				break;
			case 0x74:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_zero);

				(read_imm8s < 0) ? Logging_LogChannel("JE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x75:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, !cpu_8086.flag_zero);

				(read_imm8s < 0) ? Logging_LogChannel("JNE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JNE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x76:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_zero || cpu_8086.flag_carry);

				(read_imm8s < 0) ? Logging_LogChannel("JBE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JBE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x77:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_zero || cpu_8086.flag_carry);

				(read_imm8s < 0) ? Logging_LogChannel("JA -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JA +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x78:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_sign);

				(read_imm8s < 0) ? Logging_LogChannel("JS -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JS +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x79:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, !cpu_8086.flag_sign);

				(read_imm8s < 0) ? Logging_LogChannel("JNS -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JNS +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x7A:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_parity);

				(read_imm8s < 0) ? Logging_LogChannel("JPE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JPE +%02Xh", LogChannel_Debug, read_imm8s);
				cpu_8086.IP++;
				break;
			case 0x7B:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, !cpu_8086.flag_parity);

				(read_imm8s < 0) ? Logging_LogChannel("JPO -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JPO +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x7C:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_sign != cpu_8086.flag_overflow);

				(read_imm8s < 0) ? Logging_LogChannel("JL -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JL +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x7D:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_sign == cpu_8086.flag_overflow);

				(read_imm8s < 0) ? Logging_LogChannel("JGE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JGE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x7E:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, cpu_8086.flag_zero
					|| (cpu_8086.flag_parity != cpu_8086.flag_overflow));

				(read_imm8s < 0) ? Logging_LogChannel("JLE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JLE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x7F:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_JumpConditional(read_imm8s, !cpu_8086.flag_zero
					&& (cpu_8086.flag_parity == cpu_8086.flag_overflow));

				(read_imm8s < 0) ? Logging_LogChannel("JG -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JG +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0x90:
				// lol
				Logging_LogChannel("NOP", LogChannel_Debug);
				cpu_8086.IP++;
				break;
			case 0x9A:

				i8086_Push(cpu_8086.IP + 4); // return address after call
				i8086_Push(cpu_8086.CS);     // and the segment

				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				read_imm16u_02 = i8086_ReadU16(cpu_8086._PC += 2);

				cpu_8086.IP = read_imm16u_01;
				cpu_8086.CS = read_imm16u_02;

				Logging_LogChannel("CALL FAR %04x:%04x", LogChannel_Debug, read_imm16u_01, read_imm16u_02);

				break;
				// b0-bf: move immediate instructions
			case 0xB0:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.AL = read_imm8u;
				Logging_LogChannel("MOV AL, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB1:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.CL = read_imm8u;
				Logging_LogChannel("MOV CL, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB2:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.DL = read_imm8u;
				Logging_LogChannel("MOV DL, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB3:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.BL = read_imm8u;
				Logging_LogChannel("MOV BL, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB4:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.AH = read_imm8u;
				Logging_LogChannel("MOV AH, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB5:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.CH = read_imm8u;
				Logging_LogChannel("MOV CH, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB6:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.DH = read_imm8u;
				Logging_LogChannel("MOV DH, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB7:
				read_imm8u = i8086_ReadU8(cpu_8086._PC++);
				cpu_8086.BH = read_imm8u;
				Logging_LogChannel("MOV BH, %02x", LogChannel_Debug, read_imm8u);
				cpu_8086.IP++;
				break;
			case 0xB8:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.AX = read_imm16u_01;
				Logging_LogChannel("MOV AX, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xB9:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.CX = read_imm16u_01;
				Logging_LogChannel("MOV CX, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xBA:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.DX = read_imm16u_01;
				Logging_LogChannel("MOV DX, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xBB:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.BX = read_imm16u_01;
				Logging_LogChannel("MOV BX, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xBC:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.SP = read_imm16u_01;
				Logging_LogChannel("MOV SP, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xBD:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.BP = read_imm16u_01;
				Logging_LogChannel("MOV BP, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xBE:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.SI = read_imm16u_01;
				Logging_LogChannel("MOV SI, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
				break;
			case 0xBF:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				cpu_8086.DI = read_imm16u_01;
				Logging_LogChannel("MOV DI, %04x", LogChannel_Debug, read_imm16u_01);
				cpu_8086.IP += 2;
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
			case 0xE0:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(read_imm8s, cpu_8086.CX > 0);

				(read_imm8s < 0) ? Logging_LogChannel("LOOP -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("LOOP +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0xE1:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(read_imm8s, cpu_8086.CX > 0 && cpu_8086.flag_zero);

				(read_imm8s < 0) ? Logging_LogChannel("LOOPE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("LOOPE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0xE2:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(read_imm8s, cpu_8086.CX > 0 && !cpu_8086.flag_zero);

				(read_imm8s < 0) ? Logging_LogChannel("LOOPNE -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("LOOPNE +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0xE3:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(read_imm8s, cpu_8086.CX == 0);

				(read_imm8s < 0) ? Logging_LogChannel("JCXZ -%02Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JCXZ +%02Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0xE8:
				i8086_Push(cpu_8086._PC + 4); // return address after call

				read_imm16s_01 = i8086_ReadS16(cpu_8086._PC++);

				cpu_8086.IP += read_imm16s_01;
				(read_imm8s < 0) ? Logging_LogChannel("CALL NEAR -%04Xh", LogChannel_Debug, read_imm16s_01) : Logging_LogChannel("CALL +%04Xh", LogChannel_Debug, read_imm16s_01);
				break;
			case 0xE9:
				read_imm16s_01 = i8086_ReadS16(cpu_8086._PC++);

				cpu_8086.IP += read_imm16s_01;
				(read_imm8s < 0) ? Logging_LogChannel("JMP NEAR -%04Xh", LogChannel_Debug, read_imm8s) : Logging_LogChannel("JMP +%04Xh", LogChannel_Debug, read_imm8s);
				break;
			case 0xEA:
				read_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				read_imm16u_02 = i8086_ReadU16(cpu_8086._PC += 2);

				cpu_8086.IP = read_imm16u_01;
				cpu_8086.CS = read_imm16u_02;

				Logging_LogChannel("JMP FAR %04x:%04x", LogChannel_Debug, read_imm16u_01, read_imm16u_02);

				break;
			case 0xEB:
				read_imm8s = i8086_ReadS8(cpu_8086._PC++);

				(read_imm8s < 0) ? Logging_LogChannel("JMP NEAR -%04Xh", LogChannel_Debug, read_imm16s_01) : Logging_LogChannel("JMP +%04Xh", LogChannel_Debug, read_imm16s_01);

				cpu_8086.IP += read_imm8s;
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
			case 0xFC:
				Logging_LogChannel("CLD", LogChannel_Debug);
				cpu_8086.flag_direction = false;
				cpu_8086.IP++;
				break;
			case 0xFD:
				Logging_LogChannel("CLD", LogChannel_Debug);
				cpu_8086.flag_direction = false;
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
