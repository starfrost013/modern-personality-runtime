#include "8086.h"
#include "util/logging.h"
#include "cpu/machine.h"

#include <stdint.h>
#include <stdbool.h>

// 8086.c: Main 8086 core and basic instructions
//
// This code sucks...

i8086_t cpu_8086;

void i8086_DecodeAndExecuteOpcode(uint8_t opcode);		// Internal function that decodes and executes an opcode

void i8086_Init()
{
	Logging_LogChannel("The CPU is coming up.", LogChannel_Debug);
	// TODO: Set CPU power-on state...
}

void i8086_Update()
{
	Logging_LogChannel("Starting 8086 core", LogChannel_Debug);

	// declared here so we can repeat opcodes for the REP prefix
	uint8_t next_opcode = 0x00;

	while (machine_running)
	{
		// if we are doing this
		while (!cpu_8086.halted)
		{
			// set fake PC and real SP registers (this simplifies code later down the line)
			cpu_8086._PC = (cpu_8086.CS * X86_PARAGRAPH_SIZE) + cpu_8086.IP;
			cpu_8086._realSP = (cpu_8086.SS * X86_PARAGRAPH_SIZE) + cpu_8086.SP;
			cpu_8086.last_prefix_segment = override_none; // reset prefix

			// see if we need to repeat the last opcode

			// Treat TF and Int3 as the same for now on debug builds (change this)
#if X86_DEBUG
			cpu_8086.int3 = cpu_8086.flag_trap;

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
			if (cpu_8086.last_prefix_repeat == repeat_none)
			{
				next_opcode = i8086_ReadU8(cpu_8086._PC++);
			}
			else
			{
				//TODO: DO NOTHING, if it is NOT a string opcode (THIS PREFIX IS NOT ORTHOGONAL)

				// execute the SAME opcode again, because we're repeating it
				// we read the rep prefix, perform the intended instructon and then execute
				i8086_DecodeAndExecuteOpcode(next_opcode);

				// all cases: CX terminates when it reaches zero
				cpu_8086.CX--;

				switch (cpu_8086.last_prefix_repeat)
				{
				case repeat_zero: // repe
					// ZF only checked for CMPS/SCAS
					if (next_opcode == 0xA6
						|| next_opcode == 0xA7
						|| next_opcode == 0xAE
						|| next_opcode == 0xAF
						&& cpu_8086.flag_zero)
					{
						cpu_8086.last_prefix_repeat = repeat_zero;
						continue; // break out with whatever CX we had
					}
					break;
				case repeat_nonzero:
					if (!cpu_8086.flag_zero)
					{
						cpu_8086.last_prefix_repeat = repeat_zero;
						continue;
					}
					break;
				}

				if (cpu_8086.CX == 0) // do we repeat one more time with it as 0?
					cpu_8086.last_prefix_repeat = repeat_zero;
			}


			bool increment = false;

			// handle prefixes etc
			switch (next_opcode)
			{
			case 0x26:
				cpu_8086.last_prefix_segment = override_es;
				increment = true;
				break;
			case 0x2E:
				cpu_8086.last_prefix_segment = override_cs;
				increment = true;
				break;
			case 0x36:
				cpu_8086.last_prefix_segment = override_ss;
				increment = true;
				break;
			case 0x3E:
				cpu_8086.last_prefix_segment = override_ds;
				increment = true;
				break;
			case 0xF2:
				cpu_8086.last_prefix_repeat = repeat_zero;
				increment = true;
				break;
			case 0xF3:
				cpu_8086.last_prefix_repeat = repeat_nonzero;
				increment = true;
				break;
			}

			//TODO: REPEAT PARSING

			if (increment)
			{
				next_opcode = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.IP++;
				cpu_8086._PC++;
			}

			// See if we need to repeat the next opcode or not

			i8086_DecodeAndExecuteOpcode(next_opcode);
		}

		// machine is halted but still running

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

void i8086_DecodeAndExecuteOpcode(uint8_t opcode)
{
	// start parsing

	uint16_t		interrupt_num = 0x00;
	uint8_t			imm8u = 0x00;
	int8_t			imm8s = 0x00;
	int16_t			imm16s_01 = 0x00;
	int16_t			imm16s_02 = 0x00;
	uint16_t		imm16u_01 = 0x00;
	uint16_t		imm16u_02 = 0x00;
	i8086_modrm_t	modrm_info;

	switch (opcode)
	{
	case 0x00:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add8(modrm_info.final_offset, modrm_info.reg_ptr8, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x01:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add16(modrm_info.final_offset, (uint16_t*)modrm_info.reg_ptr16, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x02:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add8(modrm_info.reg_ptr8, modrm_info.final_offset, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x03:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add16(modrm_info.reg_ptr16, modrm_info.final_offset, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x04:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		i8086_Add8(&cpu_8086.AL, &imm8u, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x05:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		i8086_Add16(&cpu_8086.AX, &imm16u_01, false);
		// we read a 16bit value
		cpu_8086.IP += 3;

		Logging_LogChannel("ADD AX, %04Xh", LogChannel_Debug, imm8u);
		break;
	case 0x06:
		i8086_Push(cpu_8086.ES);
		cpu_8086.IP++;

		Logging_LogChannel("PUSH ES", LogChannel_Debug);
		break;
	case 0x07:
		cpu_8086.ES = i8086_Pop();
		cpu_8086.IP++;

		Logging_LogChannel("POP ES", LogChannel_Debug);
		break;
	case 0x08:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Or8(modrm_info.final_offset, modrm_info.reg_ptr8);
		cpu_8086.IP += 2;

		Logging_LogChannel("OR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x09:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC += 2;
		modrm_info = i8086_ModRM(opcode, imm16u_01);

		i8086_Or16(modrm_info.final_offset, modrm_info.reg_ptr16);
		cpu_8086.IP += 3; // 2 byte instruction

		Logging_LogChannel("OR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x0A:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Or8(modrm_info.reg_ptr8, modrm_info.final_offset);
		cpu_8086.IP += 2; // 2 byte instruction

		Logging_LogChannel("OR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x0B:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC += 2;
		modrm_info = i8086_ModRM(opcode, imm16u_01);

		i8086_Or16(modrm_info.reg_ptr16, modrm_info.final_offset);
		cpu_8086.IP += 3; // 2 byte instruction

		Logging_LogChannel("OR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x0C:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Or8(&cpu_8086.AL, &imm8u);
		cpu_8086.IP += 2; // 2 byte instruction

		Logging_LogChannel("OR AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x0D:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC += 2;

		i8086_Or16(&cpu_8086.AX, &imm16u_01);
		cpu_8086.IP += 3; // 3 byte instruction

		Logging_LogChannel("OR AX, %04Xh", LogChannel_Debug, imm16u_01);
		break;
	case 0x0E:
		i8086_Push(cpu_8086.CS);
		cpu_8086.IP++;

		Logging_LogChannel("PUSH CS", LogChannel_Debug);
		break;
	case 0x0F:
		cpu_8086.CS = i8086_Pop();
		cpu_8086.IP++;

		Logging_LogChannel("POP CS (Something has gone wrong...)", LogChannel_Warning);
		break;
	case 0x10:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add8(modrm_info.final_offset, modrm_info.reg_ptr8, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x11:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add16(modrm_info.final_offset, (uint16_t*)modrm_info.reg_ptr16, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x12:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add8(modrm_info.reg_ptr8, modrm_info.final_offset, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x13:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Add16(modrm_info.reg_ptr16, (uint16_t*)modrm_info.final_offset, true);
		cpu_8086.IP += 2;
		Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x14:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		i8086_Add8(&cpu_8086.AL, &imm8u, true);

		Logging_LogChannel("ADC AL, %02Xh", LogChannel_Debug);
		cpu_8086.IP += 2;
		break;
	case 0x15:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);

		i8086_Add16(&cpu_8086.AX, &imm16u_01, true);
		Logging_LogChannel("ADC AX, %04Xh", LogChannel_Debug);

		// INCREMENT by instruction length
		cpu_8086.IP += 3;
		break;
	case 0x16:
		Logging_LogChannel("PUSH SS", LogChannel_Debug);
		i8086_Push(cpu_8086.SS);
		cpu_8086.IP += 2;
		break;
	case 0x17:
		Logging_LogChannel("POP SS", LogChannel_Debug);
		cpu_8086.SS = i8086_Pop();
		cpu_8086.IP += 2;
		break;
	case 0x18:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub8(modrm_info.final_offset, modrm_info.reg_ptr8, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x19:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub16(modrm_info.final_offset, (uint16_t*)modrm_info.reg_ptr16, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x1A:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub8(modrm_info.reg_ptr8, modrm_info.final_offset, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x1B:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub16(modrm_info.reg_ptr16, (uint16_t*)modrm_info.final_offset, true);
		cpu_8086.IP += 2;

		Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x1C:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Sub8(&cpu_8086.AL, &imm8u, true);
		cpu_8086.IP += 2;
		Logging_LogChannel("SBB AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x1D:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Sub16(&cpu_8086.AX, &imm8u, true);
		cpu_8086.IP += 3;
		Logging_LogChannel("SBB AX, %04Xh", LogChannel_Debug, imm16u_01);
		break;
	case 0x1E:
		Logging_LogChannel("PUSH DS", LogChannel_Debug);
		i8086_Push(cpu_8086.DS);
		cpu_8086.IP++;
		break;
	case 0x1F:
		Logging_LogChannel("POP DS", LogChannel_Debug);
		cpu_8086.DS = i8086_Pop();
		cpu_8086.IP++;
		break;
		// AND block
	case 0x20:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_And8(modrm_info.final_offset, modrm_info.reg_ptr8);
		cpu_8086.IP += 2;

		Logging_LogChannel("AND %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x21:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_And16(modrm_info.final_offset, modrm_info.reg_ptr16);
		cpu_8086.IP += 3;

		Logging_LogChannel("AND %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x22:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_And8(modrm_info.reg_ptr8, modrm_info.final_offset);
		cpu_8086.IP += 2;

		Logging_LogChannel("AND %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x23:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC += 2;
		modrm_info = i8086_ModRM(opcode, imm16u_01);

		i8086_And16(modrm_info.reg_ptr16, modrm_info.final_offset);
		cpu_8086.IP += 3;

		Logging_LogChannel("AND %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x24:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_And8(&cpu_8086.AL, imm8u);
		cpu_8086.IP += 2;

		Logging_LogChannel("AND AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x25:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC += 2;

		i8086_And16(&cpu_8086.AX, imm16u_01);
		cpu_8086.IP += 3;

		Logging_LogChannel("AND AX, %04Xh", LogChannel_Debug, imm16u_01);
		break;
	case 0x28:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub8(modrm_info.final_offset, modrm_info.reg_ptr8, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x29:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub16(modrm_info.final_offset, modrm_info.reg_ptr16, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x2A:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub8(modrm_info.reg_ptr8, modrm_info.final_offset, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x2B:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Sub16(modrm_info.reg_ptr16, modrm_info.final_offset, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x2C:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Sub8(&cpu_8086.AL, &imm8u, false);
		cpu_8086.IP += 2;

		Logging_LogChannel("SUB AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x2D:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Sub16(&cpu_8086.AX, &imm8u, false);
		cpu_8086.IP += 3;

		Logging_LogChannel("SUB AX, %04Xh", LogChannel_Debug, imm16u_01);
		break;
	case 0x30:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);
		i8086_Xor8(modrm_info.final_offset, modrm_info.reg_ptr8);
		cpu_8086.IP += 2;

		Logging_LogChannel("XOR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x31:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);
		i8086_Xor16(modrm_info.final_offset, modrm_info.reg_ptr16);
		cpu_8086.IP += 2;

		Logging_LogChannel("XOR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x32:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);
		i8086_Xor8(modrm_info.reg_ptr8, modrm_info.final_offset);
		cpu_8086.IP += 2;

		Logging_LogChannel("XOR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x33:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);
		i8086_Xor16(modrm_info.reg_ptr16, modrm_info.final_offset);
		cpu_8086.IP += 2;

		Logging_LogChannel("XOR %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x34:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		i8086_Xor8(&cpu_8086.AL, imm8u);
		cpu_8086.IP += 2;

		Logging_LogChannel("XOR AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x35:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC++;
		i8086_Xor8(&cpu_8086.AX, imm16u_01);
		cpu_8086.IP += 3;

		Logging_LogChannel("XOR AX, %04Xh", LogChannel_Debug, imm8u);
		break;
	case 0x38:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Cmp8(modrm_info.final_offset, modrm_info.reg_ptr8);
		cpu_8086.IP += 2;

		Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x39:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Cmp16(modrm_info.final_offset, modrm_info.reg_ptr16);
		cpu_8086.IP += 2;

		Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x3A:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Cmp8(modrm_info.reg_ptr8, modrm_info.final_offset);
		cpu_8086.IP += 2;

		Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x3B:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Cmp16(modrm_info.reg_ptr16, modrm_info.final_offset);
		cpu_8086.IP += 2;

		Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x3C:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Cmp8(&cpu_8086.AL, &imm8u);
		cpu_8086.IP += 2;
		Logging_LogChannel("CMP AL, %02Xh", LogChannel_Debug, imm8u);
		break;
	case 0x3D:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC++;

		i8086_Cmp16(&cpu_8086.AX, &imm16u_01);
		cpu_8086.IP += 3;
		Logging_LogChannel("CMP AX, %04Xh", LogChannel_Debug, imm16u_01);
		break;
	case 0x40:
		cpu_8086.AX++;
		cpu_8086.IP++;
		Logging_LogChannel("INC AX", LogChannel_Debug);
		break;
	case 0x41:
		cpu_8086.BX++;
		cpu_8086.IP++;
		Logging_LogChannel("INC BX", LogChannel_Debug);
		break;
	case 0x42:
		cpu_8086.CX++;
		cpu_8086.IP++;
		Logging_LogChannel("INC CX", LogChannel_Debug);
		break;
	case 0x43:
		cpu_8086.DX++;
		cpu_8086.IP++;
		Logging_LogChannel("INC DX", LogChannel_Debug);
		break;
	case 0x44:
		cpu_8086.SP++;
		cpu_8086.IP++;
		Logging_LogChannel("INC SP", LogChannel_Debug);
		break;
	case 0x45:
		cpu_8086.BP++;
		cpu_8086.IP++;
		Logging_LogChannel("INC BP", LogChannel_Debug);
		break;
	case 0x46:
		cpu_8086.SI++;
		cpu_8086.IP++;
		Logging_LogChannel("INC SI", LogChannel_Debug);
		break;
	case 0x47:
		cpu_8086.DI++;
		cpu_8086.IP++;
		Logging_LogChannel("INC DI", LogChannel_Debug);
		break;
	case 0x48:
		cpu_8086.AX--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC AX", LogChannel_Debug);
		break;
	case 0x49:
		cpu_8086.BX--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC BX", LogChannel_Debug);
		break;
	case 0x4A:
		cpu_8086.CX--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC CX", LogChannel_Debug);
		break;
	case 0x4B:
		cpu_8086.DX--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC DX", LogChannel_Debug);
		break;
	case 0x4C:
		cpu_8086.SP--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC SP", LogChannel_Debug);
		break;
	case 0x4D:
		cpu_8086.BP--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC BP", LogChannel_Debug);
		break;
	case 0x4E:
		cpu_8086.SI--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC SI", LogChannel_Debug);
		break;
	case 0x4F:
		cpu_8086.DI--;
		cpu_8086.IP++;
		Logging_LogChannel("DEC DI", LogChannel_Debug);
		break;
	case 0x50:
		i8086_Push(cpu_8086.AX);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH AX", LogChannel_Debug);
		break;
	case 0x51:
		i8086_Push(cpu_8086.CX);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH CX", LogChannel_Debug);
		break;
	case 0x52:
		i8086_Push(cpu_8086.DX);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH DX", LogChannel_Debug);
		break;
	case 0x53:
		i8086_Push(cpu_8086.BX);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH BX", LogChannel_Debug);
		break;
	case 0x54:
		i8086_Push(cpu_8086.SP);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH SP", LogChannel_Debug);
		break;
	case 0x55:
		i8086_Push(cpu_8086.BP);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH BP", LogChannel_Debug);
		break;
	case 0x56:
		i8086_Push(cpu_8086.SI);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH SI", LogChannel_Debug);
		break;
	case 0x57:
		i8086_Push(cpu_8086.DI);
		cpu_8086.IP++;
		Logging_LogChannel("PUSH DI", LogChannel_Debug);
		break;
	case 0x58:
		cpu_8086.AX = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP AX", LogChannel_Debug);
		break;
	case 0x59:
		cpu_8086.CX = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP CX", LogChannel_Debug);
		break;
	case 0x5A:
		cpu_8086.DX = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP DX", LogChannel_Debug);
		break;
	case 0x5B:
		cpu_8086.BX = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP BX", LogChannel_Debug);
		break;
	case 0x5C:
		cpu_8086.SP = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP SP", LogChannel_Debug);
		break;
	case 0x5D:
		cpu_8086.BP = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP BP", LogChannel_Debug);
		break;
	case 0x5E:
		cpu_8086.SI = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP SI", LogChannel_Debug);
		break;
	case 0x5F:
		cpu_8086.DI = i8086_Pop();
		cpu_8086.IP++;
		Logging_LogChannel("POP DI", LogChannel_Debug);
		break;
	case 0x60: // undocumented alias due to microcode bit matching criteria
	case 0x70:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_overflow);

		(imm8s < 0) ? Logging_LogChannel("JO -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JO +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x61: // undocumented alias due to microcode bit matching criteria
	case 0x71:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, !cpu_8086.flag_overflow);

		// print the direction we want to change
		(imm8s < 0) ? Logging_LogChannel("JNO -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JNO +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x62: // undocumented alias due to microcode bit matching criteria
	case 0x72:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_carry);

		(imm8s < 0) ? Logging_LogChannel("JB -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JB +%02Xh", LogChannel_Debug, imm8s);

		break;
	case 0x63: // undocumented alias due to microcode bit matching criteria
	case 0x73:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, !cpu_8086.flag_carry);

		(imm8s < 0) ? Logging_LogChannel("JAE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JAE +%02Xh", LogChannel_Debug, imm8s);

		break;
	case 0x64: // undocumented alias due to microcode bit matching criteria
	case 0x74:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_zero);

		(imm8s < 0) ? Logging_LogChannel("JE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x65: // undocumented alias due to microcode bit matching criteria
	case 0x75:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, !cpu_8086.flag_zero);

		(imm8s < 0) ? Logging_LogChannel("JNE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JNE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x66: // undocumented alias due to microcode bit matching criteria
	case 0x76:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_zero || cpu_8086.flag_carry);

		(imm8s < 0) ? Logging_LogChannel("JBE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JBE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x67: // undocumented alias due to microcode bit matching criteria
	case 0x77:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_zero || cpu_8086.flag_carry);

		(imm8s < 0) ? Logging_LogChannel("JA -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JA +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x68: // undocumented alias due to microcode bit matching criteria
	case 0x78:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_sign);

		(imm8s < 0) ? Logging_LogChannel("JS -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JS +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x69: // undocumented alias due to microcode bit matching criteria
	case 0x79:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, !cpu_8086.flag_sign);

		(imm8s < 0) ? Logging_LogChannel("JNS -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JNS +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x6A: // undocumented alias due to microcode bit matching criteria
	case 0x7A:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_parity);

		(imm8s < 0) ? Logging_LogChannel("JPE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JPE +%02Xh", LogChannel_Debug, imm8s);
		cpu_8086.IP++;
		break;
	case 0x6B: // undocumented alias due to microcode bit matching criteria
	case 0x7B:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, !cpu_8086.flag_parity);

		(imm8s < 0) ? Logging_LogChannel("JPO -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JPO +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x6C: // undocumented alias due to microcode bit matching criteria
	case 0x7C:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_sign != cpu_8086.flag_overflow);

		(imm8s < 0) ? Logging_LogChannel("JL -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JL +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x6D: // undocumented alias due to microcode bit matching criteria
	case 0x7D:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_sign == cpu_8086.flag_overflow);

		(imm8s < 0) ? Logging_LogChannel("JGE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JGE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x6E: // undocumented alias due to microcode bit matching criteria
	case 0x7E:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, cpu_8086.flag_zero
			|| (cpu_8086.flag_parity != cpu_8086.flag_overflow));

		(imm8s < 0) ? Logging_LogChannel("JLE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JLE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x6F: // undocumented alias due to microcode bit matching criteria
	case 0x7F:
		imm8s = i8086_ReadS8(cpu_8086._PC);
		i8086_JumpConditional(imm8s, !cpu_8086.flag_zero
			&& (cpu_8086.flag_parity == cpu_8086.flag_overflow));

		(imm8s < 0) ? Logging_LogChannel("JG -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JG +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0x80: // grp1
	case 0x81:
	case 0x82:
	case 0x83:
		i8086_Grp1(opcode);
		break;
		// ModRM test/xchg
	case 0x84:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Test8(modrm_info.final_offset, modrm_info.reg_ptr8);

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("TEST %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x85:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Test16(modrm_info.final_offset, modrm_info.reg_ptr16);

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("TEST %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x86:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		imm8u = *modrm_info.reg_ptr8;
		*modrm_info.reg_ptr8 = *modrm_info.final_offset;
		*modrm_info.final_offset = imm8u;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("XCHG %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x87:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		imm16u_01 = *modrm_info.reg_ptr16;
		*modrm_info.reg_ptr8 = *modrm_info.final_offset;
		*modrm_info.final_offset = imm16u_01;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("XCHG %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x88:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.final_offset = *modrm_info.reg_ptr8;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x89:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.final_offset = *modrm_info.reg_ptr16;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x8A:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.reg_ptr8 = &modrm_info.final_offset;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x8B:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.reg_ptr16 = *modrm_info.final_offset;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);

		break;
	case 0x8C:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.final_offset = *modrm_info.reg_ptr16;

		cpu_8086.IP += 2; // 1 modrm byte

		Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x8E:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.reg_ptr16 = *modrm_info.final_offset;

		cpu_8086.IP += 2; // 1 modrm byte
		Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
		break;
	case 0x8F:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		*modrm_info.final_offset = i8086_Pop();

		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0x90:
		// lol
		// this is actually XCHG AX, AX
		// does anything depend on this behaviour?
		Logging_LogChannel("NOP", LogChannel_Debug);
		cpu_8086.IP++;
		break;
	case 0x91:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.CX;
		cpu_8086.CX = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG CX, AX", LogChannel_Debug);
		break;
	case 0x92:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.DX;
		cpu_8086.DX = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG DX, AX", LogChannel_Debug);
		break;
	case 0x93:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.BX;
		cpu_8086.BX = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG BX, AX", LogChannel_Debug);
		break;
	case 0x94:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.SP;
		cpu_8086.SP = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG SP, AX", LogChannel_Debug);
		break;
	case 0x95:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.BP;
		cpu_8086.BP = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG BP, AX", LogChannel_Debug);
		break;
	case 0x96:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.SI;
		cpu_8086.SI = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG SI, AX", LogChannel_Debug);
		break;
	case 0x97:
		imm16u_01 = cpu_8086.AX;
		cpu_8086.AX = cpu_8086.DI;
		cpu_8086.DI = imm16u_01;
		cpu_8086.IP++;
		Logging_LogChannel("XCHG DI, AX", LogChannel_Debug);
		break;

	case 0x9A: // CALL FAR
		i8086_Push(cpu_8086.IP + 4); // offset of return address after call
		i8086_Push(cpu_8086.CS);     // and the segment

		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		imm16u_02 = i8086_ReadU16(cpu_8086._PC += 2);

		cpu_8086.IP = imm16u_01;
		cpu_8086.CS = imm16u_02;

		Logging_LogChannel("CALL FAR %04Xh:%04Xh", LogChannel_Debug, imm16u_01, imm16u_02);
		break;
	case 0x9C: // PUSHF
		i8086_Pushf();
		break;
	case 0x9D: // POPF
		i8086_Popf();
		break;
	case 0x9E: // SAHF
		cpu_8086.flag_sign = (cpu_8086.AH) & 0x80;			//bit0
		cpu_8086.flag_zero = (cpu_8086.AH) & 0x40;			//bit1
		//bit2 - reserved, always 0
		cpu_8086.flag_aux_carry = (cpu_8086.AH) & 0x10;		//bit3
		//bit4 - reserved, always 0
		cpu_8086.flag_parity = (cpu_8086.AH) & 0x04;		//bit5
		//bit6 - reserved, always 1, see above
		cpu_8086.flag_carry = (cpu_8086.AH) & 0x01;			//bit7

		Logging_LogChannel("SAHF", LogChannel_Debug);
		cpu_8086.IP++;
		break;
	case 0x9F: // LAHF
		cpu_8086.AH = 0x02; // Set initial AH to 0b00000010. Bit1 is always 1, so load it here.

		if (cpu_8086.flag_sign) cpu_8086.AH |= 0x80;		//bit0
		if (cpu_8086.flag_zero) cpu_8086.AH |= 0x40;		//bit1
		//bit2 - reserved, always 0
		if (cpu_8086.flag_aux_carry) cpu_8086.AH |= 0x10;	//bit3
		//bit4 - reserved, always 0
		if (cpu_8086.flag_parity) cpu_8086.AH |= 0x04;		//bit5
		//bit6 - reserved, always 1, see above
		if (cpu_8086.flag_carry) cpu_8086.AH |= 0x01;		//bit7
		Logging_LogChannel("LAHF", LogChannel_Debug);
		cpu_8086.IP++;
		break;
		// a0-a3: bizarre special case instructions
	case 0xA0:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		i8086_MoveSegOff8(imm8u, false);
		break;
	case 0xA1:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		i8086_MoveSegOff16(imm16u_01, false);
		break;
		// a0-a3: bizarre special case instructions
	case 0xA2:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		i8086_MoveSegOff8(imm8u, true);
		break;
	case 0xA3:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		i8086_MoveSegOff16(imm16u_01, true);
		break;
		// string hell
	case 0xA4:
		i8086_Movsb();
		break;
	case 0xA5:
		i8086_Movsw();
		break;
	case 0xA6:
		i8086_Cmpsb();
		break;
	case 0xA7:
		i8086_Cmpsw();
		break;
	case 0xA8: // dubous implementation
		modrm_info = i8086_ModRM(opcode, imm8u);

		i8086_Test8(modrm_info.reg_ptr8, &imm8u);

		Logging_LogChannel("TEST %s, %xh", LogChannel_Debug, modrm_info.disasm, imm8u);

		cpu_8086.IP += 2;

		break;
	case 0xA9:
		modrm_info = i8086_ModRM(opcode, imm16u_01);

		i8086_Test8(modrm_info.final_offset, &imm16u_01);

		Logging_LogChannel("TEST %s", LogChannel_Debug, modrm_info.disasm);

		cpu_8086.IP += 2;
		break;
	case 0xAA:
		i8086_Stosb();
		break;
	case 0xAB:
		i8086_Stosw();
		break;
	case 0xAC:
		i8086_Lodsb();
		break;
	case 0xAD:
		i8086_Lodsw();
		break;
	case 0xAE:
		i8086_Scasb();
		break;
	case 0xAF:
		i8086_Scasw();
		break;
		// b0-bf: 16-bit move immediate instructions
	case 0xB0:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.AL = imm8u;
		Logging_LogChannel("MOV AL, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB1:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.CL = imm8u;
		Logging_LogChannel("MOV CL, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB2:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.DL = imm8u;
		Logging_LogChannel("MOV DL, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB3:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.BL = imm8u;
		Logging_LogChannel("MOV BL, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB4:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.AH = imm8u;
		Logging_LogChannel("MOV AH, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB5:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.CH = imm8u;
		Logging_LogChannel("MOV CH, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB6:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.DH = imm8u;
		Logging_LogChannel("MOV DH, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB7:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086.BH = imm8u;
		Logging_LogChannel("MOV BH, %02Xh", LogChannel_Debug, imm8u);
		cpu_8086.IP += 2; // 1 modrm byte
		break;
	case 0xB8:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.AX = imm16u_01;
		Logging_LogChannel("MOV AX, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xB9:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.CX = imm16u_01;
		Logging_LogChannel("MOV CX, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xBA:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.DX = imm16u_01;
		Logging_LogChannel("MOV DX, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xBB:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.BX = imm16u_01;
		Logging_LogChannel("MOV BX, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xBC:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.SP = imm16u_01;
		Logging_LogChannel("MOV SP, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xBD:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.BP = imm16u_01;
		Logging_LogChannel("MOV BP, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xBE:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.SI = imm16u_01;
		Logging_LogChannel("MOV SI, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xBF:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);
		cpu_8086.DI = imm16u_01;
		Logging_LogChannel("MOV DI, %04Xh", LogChannel_Debug, imm16u_01);
		cpu_8086.IP += 3;
		break;
	case 0xC0: // undocumented alias
	case 0xC2:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);

		// pull return address
		uint16_t ret_address = i8086_Pop();

		// operand2 is number of bytes to free from stack
		// for parameters etc..
		for (int32_t i = 0; i < imm16u_01; i += 2)
		{
			// throw away
			i8086_Pop();
		}

		cpu_8086.IP = ret_address;
		Logging_LogChannel("RETN CS:%04Xh", LogChannel_Debug, imm16u_01);
		break;
	case 0xC1:
	case 0xC3:
		// pull return address for IP
		cpu_8086.IP = i8086_Pop();
		Logging_LogChannel("NEAR RET to %04Xh:%04Xh", LogChannel_Debug, cpu_8086.CS, cpu_8086.IP);
		break;
	case 0xC4: // LDS - load [DS:r16] with far pointer
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm16u_01);

		imm16u_01 = i8086_ReadU16(cpu_8086._PC);	// Segment
		cpu_8086._PC += 2;
		imm16u_02 = i8086_ReadU16(cpu_8086._PC);	// Offset

		// calculate the final area to load from
		int32_t final_location = (imm16u_01 * X86_PARAGRAPH_SIZE) + imm16u_02;

		// convert to 16 bit ptr and load 16-bit register from ModR/M info with the pointer we loaded

		*modrm_info.reg_ptr16 = *(uint16_t*)(cpu_8086.address_space[final_location]);

		// load the segment register with the high bytes
		cpu_8086.DS = *(uint16_t*)(cpu_8086.address_space[final_location + 2]);

		Logging_LogChannel("LDS %s, %04Xh:%04Xh", LogChannel_Debug, modrm_info.disasm, imm16u_01, imm16u_02);

		// next instruction
		cpu_8086.IP += 5;
		break;
	case 0xC5: // LES - load [ES:r16] with far pointer
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm16u_01);

		imm16u_01 = i8086_ReadU16(cpu_8086._PC);	// Segment
		cpu_8086._PC += 2;
		imm16u_02 = i8086_ReadU16(cpu_8086._PC);	// Offset

		// calculate the final area to load from
		int32_t ptr_location = (imm16u_01 * X86_PARAGRAPH_SIZE) + imm16u_02;

		// convert to 16 bit ptr and load 16-bit register from ModR/M info with the pointer we loaded

		*modrm_info.reg_ptr16 = *(uint16_t*)(cpu_8086.address_space[final_location]);

		// load the segment register with the high bytes
		cpu_8086.ES = *(uint16_t*)(cpu_8086.address_space[final_location + 2]);

		Logging_LogChannel("LES %s, %04Xh:%04Xh", LogChannel_Debug, modrm_info.disasm, imm16u_01, imm16u_02);

		// next instruction
		cpu_8086.IP += 5;
		break;
	case 0xC6:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		// we don't need the previous temp_imm8u value
		imm8u = i8086_ReadU8(cpu_8086._PC);

		*modrm_info.reg_ptr8 = imm8u;
		cpu_8086.IP += 3;

		Logging_LogChannel("MOV %s, %02Xh", LogChannel_Debug, modrm_info.disasm, imm8u);
		break;
	case 0xC7:
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		// we don't need the previous temp_imm8u value
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);

		*modrm_info.reg_ptr16 = imm16u_01;
		cpu_8086.IP += 4;

		Logging_LogChannel("MOV %s, %04Xh", LogChannel_Debug, modrm_info.disasm, imm16u_01);
		break;
	case 0xC8: // undocumented alias
	case 0xCA:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC);

		// pull return address
		uint16_t ret_address_off = i8086_Pop();
		uint16_t ret_address_seg = i8086_Pop();

		// THIS IS DONE AFTER

		// operand2 is number of bytes to free from stack
		// for parameters etc..
		for (int32_t i = 0; i < imm16u_01; i += 2)
		{
			// throw away
			i8086_Pop();
		}

		cpu_8086.IP = ret_address_off;
		cpu_8086.CS = ret_address_seg;
		Logging_LogChannel("FAR RETN %04Xh (to %04Xh:%04Xh)", LogChannel_Debug, imm16u_01, cpu_8086.CS, cpu_8086.IP);
		break;
	case 0xC9: // undocumented alias
	case 0xCB:
		// pull return address for IP and CS
		cpu_8086.IP = i8086_Pop();
		cpu_8086.CS = i8086_Pop();
		Logging_LogChannel("FAR RET to %04Xh:%04Xh", LogChannel_Debug, cpu_8086.CS, cpu_8086.IP);
		break;
	case 0xCC:
		cpu_8086.int3 = true;
		cpu_8086.IP++;
		break;
	case 0xCD:
		i8086_Interrupt(); // call interrupt service
		break;
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
		i8086_Grp2(opcode);
		break;
	case 0xD6:
		// SALC (undocumented copyright trap instruction, intel 8086 only)
		cpu_8086.AL = (cpu_8086.flag_carry) ? 0xFF : 0x00;
		Logging_LogChannel("SALC", LogChannel_Debug);
		break;
	case 0xD7:
		i8086_Xlat();
		break;
		// Coprocessor Escape (0xD8-0xDF)
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
		Logging_LogChannel("Tried to execute Intel 8087 coprocessor escape instruction. NOT IMPLEMENTED! Faking it...", LogChannel_Warning);

		// pretend that we did that anyway
		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;
		modrm_info = i8086_ModRM(opcode, imm8u);

		// literally do nothing
		if (modrm_info.rm == 0b11)
			break;
		// two displacement bytes
		else if (modrm_info.rm == 0b10)
		{
			imm16u_01 = i8086_ReadU16(cpu_8086._PC);
			cpu_8086._PC += 2;
			break;
		}
		// one displacement byte

		imm8u = i8086_ReadU8(cpu_8086._PC);
		cpu_8086._PC++;

		// otherwise read a byte and do nothing

		Logging_LogChannel("ESC [UNIMPLEMENTED]", LogChannel_Debug, imm8s);
		break;

	case 0xE0:
		imm8s = i8086_ReadS8(cpu_8086._PC++);
		i8086_Loop(imm8s, cpu_8086.CX > 0);

		(imm8s < 0) ? Logging_LogChannel("LOOP -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("LOOP +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0xE1:
		imm8s = i8086_ReadS8(cpu_8086._PC++);
		i8086_Loop(imm8s, cpu_8086.CX > 0 && cpu_8086.flag_zero);

		(imm8s < 0) ? Logging_LogChannel("LOOPE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("LOOPE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0xE2:
		imm8s = i8086_ReadS8(cpu_8086._PC++);
		i8086_Loop(imm8s, cpu_8086.CX > 0 && !cpu_8086.flag_zero);

		(imm8s < 0) ? Logging_LogChannel("LOOPNE -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("LOOPNE +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0xE3:
		imm8s = i8086_ReadS8(cpu_8086._PC++);
		i8086_Loop(imm8s, cpu_8086.CX == 0);

		(imm8s < 0) ? Logging_LogChannel("JCXZ -%02Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JCXZ +%02Xh", LogChannel_Debug, imm8s);
		break;
	case 0xE8:
		i8086_Push(cpu_8086._PC + 4); // return address after call

		imm16s_01 = i8086_ReadS16(cpu_8086._PC++);

		cpu_8086.IP += imm16s_01;
		(imm8s < 0) ? Logging_LogChannel("CALL NEAR -%04Xh", LogChannel_Debug, imm16s_01) : Logging_LogChannel("CALL +%04Xh", LogChannel_Debug, imm16s_01);
		break;
	case 0xE9:
		imm16s_01 = i8086_ReadS16(cpu_8086._PC++);

		cpu_8086.IP += imm16s_01;
		(imm8s < 0) ? Logging_LogChannel("JMP NEAR -%04Xh", LogChannel_Debug, imm8s) : Logging_LogChannel("JMP +%04Xh", LogChannel_Debug, imm8s);
		break;
	case 0xEA:
		imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
		imm16u_02 = i8086_ReadU16(cpu_8086._PC += 2);

		cpu_8086.IP = imm16u_01;
		cpu_8086.CS = imm16u_02;

		Logging_LogChannel("JMP FAR %04Xh:%04Xh", LogChannel_Debug, imm16u_01, imm16u_02);

		break;
	case 0xEB:
		imm8s = i8086_ReadS8(cpu_8086._PC++);

		(imm8s < 0) ? Logging_LogChannel("JMP NEAR -%04Xh", LogChannel_Debug, imm16s_01) : Logging_LogChannel("JMP +%04Xh", LogChannel_Debug, imm16s_01);

		cpu_8086.IP += imm8s;
		break;
	case 0xF4: // HLT
		cpu_8086.halted = true;
		cpu_8086.IP++;
		Logging_LogChannel("HL", LogChannel_Debug);
		break;
	case 0xF5:
		// IGNORE THIS WARNING, THIS IS WHAT THE INSTRUCTION IS MEANT TO DO 
		cpu_8086.flag_carry = !cpu_8086.flag_carry; // get complement
		cpu_8086.IP++;
		Logging_LogChannel("CMC", LogChannel_Debug);
		break;
	case 0xF6: // grp3a
	case 0xF7: // grp3b
		i8086_Grp3(opcode);
		break;
	case 0xF8:
		cpu_8086.flag_carry = false;
		cpu_8086.IP++;
		Logging_LogChannel("CLC", LogChannel_Debug);
		break;
	case 0xF9:
		cpu_8086.flag_carry = true;
		cpu_8086.IP++;
		Logging_LogChannel("STC", LogChannel_Debug);
		break;
	case 0xFA:
		cpu_8086.flag_interrupt_enable = false;
		cpu_8086.IP++;
		Logging_LogChannel("CLI", LogChannel_Debug);
		break;
	case 0xFB:
		cpu_8086.flag_interrupt_enable = true;
		cpu_8086.IP++;
		Logging_LogChannel("STI", LogChannel_Debug);
		break;
	case 0xFC:
		cpu_8086.flag_direction = false;
		cpu_8086.IP++;
		Logging_LogChannel("CLD", LogChannel_Debug);
		break;
	case 0xFD:
		cpu_8086.flag_direction = false;
		cpu_8086.IP++;
		Logging_LogChannel("CLD", LogChannel_Debug);
		break;
	case 0xFE:  // grp4
		i8086_Grp4(opcode);
		break;
	case 0xFF:  // grp5
		i8086_Grp5(opcode);
		break;
	default:
		Logging_LogChannel("Unimplemented 8086 opcode 0x%X @ %04Xh:%04Xh", LogChannel_Error, opcode, cpu_8086.CS, cpu_8086.IP);
		cpu_8086.IP++;
		break;

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

uint16_t i8086_FlagsToWord()
{
	uint16_t flags = 0xF000;

	// bits 12-15 always 1 (bit 15 is 0 on a 286+)
	flags = 0xF000;

	// populate the top of the stack with the various flags
	if (cpu_8086.flag_overflow) flags |= 0x800;
	if (cpu_8086.flag_direction) flags |= 0x400;
	if (cpu_8086.flag_interrupt_enable) flags |= 0x200;
	if (cpu_8086.flag_trap) flags |= 0x100;
	if (cpu_8086.flag_sign) flags |= 0x80;
	if (cpu_8086.flag_zero) flags |= 0x40;
	//bit5 undefined per 8086 user's manual
	if (cpu_8086.flag_aux_carry) flags |= 0x10;
	if (cpu_8086.flag_parity) flags |= 0x4;
	if (cpu_8086.flag_carry) flags |= 0x1;

	return flags;
}