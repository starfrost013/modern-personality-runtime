#include "8086.h"
#include "util/logging.h"
#include "cpu/machine.h"

#include <stdint.h>
#include <stdbool.h>

i8086_t cpu_8086;

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
			cpu_8086.last_prefix = override_none; // reset prefix

			// Treat TF and Int3 as the same for now (change this)
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
			//cpu_8086.IP++;

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

			uint16_t		interrupt_num = 0x00;
			uint8_t			temp_imm8u = 0x00;
			int8_t			temp_imm8s = 0x00;
			int16_t			temp_imm16s_01 = 0x00;
			int16_t			temp_imm16s_02 = 0x00;
			uint16_t		temp_imm16u_01 = 0x00;
			uint16_t		temp_imm16u_02 = 0x00;
			i8086_modrm_t	modrm_info;

			switch (next_opcode)
			{
			case 0x00:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add8(modrm_info.final_offset, modrm_info.reg_ptr8, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x01:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add16(modrm_info.final_offset, modrm_info.reg_ptr16, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x02:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add8(modrm_info.reg_ptr8, modrm_info.final_offset, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x03:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add16(modrm_info.reg_ptr16, modrm_info.final_offset, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x04:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				i8086_Add8(&cpu_8086.AL, &temp_imm8u, false);
				Logging_LogChannel("ADD AL, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2;

#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x05:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				i8086_Add16(&cpu_8086.AX, &temp_imm16u_01, false);
				// we read a 16bit value
				cpu_8086.IP += 3;

				Logging_LogChannel("ADD AL, %04Xh", LogChannel_Debug, temp_imm8u);
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
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add8(modrm_info.final_offset, modrm_info.reg_ptr8, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x11:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add16(modrm_info.final_offset, modrm_info.reg_ptr16, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x12:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add8(modrm_info.reg_ptr8, modrm_info.final_offset, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x13:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Add16(modrm_info.reg_ptr16, modrm_info.final_offset, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("ADD %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x14:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				i8086_Add8(&cpu_8086.AL, &temp_imm8u, true);
				Logging_LogChannel("ADC AL, %02Xh", LogChannel_Debug);
				cpu_8086.IP += 2;
				break;
			case 0x15:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);

				i8086_Add16(&cpu_8086.AX, &temp_imm16u_01, true);
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
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub8(modrm_info.final_offset, modrm_info.reg_ptr8, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x19:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub16(modrm_info.final_offset, modrm_info.reg_ptr16, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x1A:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub8(modrm_info.reg_ptr8, modrm_info.final_offset, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x1B:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub16(modrm_info.reg_ptr16, modrm_info.final_offset, true);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SBB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x1C:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;

				i8086_Sub8(&cpu_8086.AL, &temp_imm8u, true);
				cpu_8086.IP += 2;
				Logging_LogChannel("SBB AL, %02Xh", LogChannel_Debug, temp_imm8u);
				break;
			case 0x1D:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086._PC++;

				i8086_Sub16(&cpu_8086.AX, &temp_imm8u, true);
				cpu_8086.IP += 3;
				Logging_LogChannel("SBB AX, %04Xh", LogChannel_Debug, temp_imm16u_01);
				break;
			case 0x1E:
				Logging_LogChannel("PUSH DS", LogChannel_Debug);
				i8086_Push(cpu_8086.ES);
				cpu_8086.IP += 2;
				break;
			case 0x1F:
				Logging_LogChannel("POP DS", LogChannel_Debug);
				cpu_8086.DS = i8086_Pop();
				cpu_8086.IP += 2;
				break;
			case 0x28:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub8(modrm_info.final_offset, modrm_info.reg_ptr8, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x29:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub16(modrm_info.final_offset, modrm_info.reg_ptr16, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x2A:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub8(modrm_info.reg_ptr8, modrm_info.final_offset, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x2B:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Sub16(modrm_info.reg_ptr16, modrm_info.final_offset, false);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("SUB %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x2C:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;

				i8086_Sub8(&cpu_8086.AL, &temp_imm8u, false);
				cpu_8086.IP += 2;
				Logging_LogChannel("SUB AL, %02Xh", LogChannel_Debug, temp_imm8u);
				break;
			case 0x2D:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086._PC++;

				i8086_Sub16(&cpu_8086.AX, &temp_imm8u, false);
				cpu_8086.IP += 3;
				Logging_LogChannel("SUB AX, %04Xh", LogChannel_Debug, temp_imm16u_01);
				break;
			case 0x38:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Cmp8(modrm_info.final_offset, modrm_info.reg_ptr8);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x39:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Cmp16(modrm_info.final_offset, modrm_info.reg_ptr16);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x3A:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Cmp8(modrm_info.reg_ptr8, modrm_info.final_offset);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x3B:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				i8086_Cmp16(modrm_info.reg_ptr16, modrm_info.final_offset);
				cpu_8086.IP += 2;
#if X86_DEBUG
				Logging_LogChannel("CMP %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x3C:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;

				i8086_Cmp8(&cpu_8086.AL, &temp_imm8u);
				cpu_8086.IP += 2;
				Logging_LogChannel("CMP AL, %02Xh", LogChannel_Debug, temp_imm8u);
				break;
			case 0x3D:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086._PC++;

				i8086_Cmp16(&cpu_8086.AX, &temp_imm8u);
				cpu_8086.IP += 3;
				Logging_LogChannel("CMP AX, %04Xh", LogChannel_Debug, temp_imm16u_01);
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
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_overflow);

				(temp_imm8s < 0) ? Logging_LogChannel("JO -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JO +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x61: // undocumented alias due to microcode bit matching criteria
			case 0x71:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, !cpu_8086.flag_overflow);

				// print the direction we want to change
				(temp_imm8s < 0) ? Logging_LogChannel("JNO -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JNO +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x62: // undocumented alias due to microcode bit matching criteria
			case 0x72:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_carry);

				(temp_imm8s < 0) ? Logging_LogChannel("JB -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JB +%02Xh", LogChannel_Debug, temp_imm8s);

				break;
			case 0x63: // undocumented alias due to microcode bit matching criteria
			case 0x73:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, !cpu_8086.flag_carry);

				(temp_imm8s < 0) ? Logging_LogChannel("JAE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JAE +%02Xh", LogChannel_Debug, temp_imm8s);

				break;
			case 0x64: // undocumented alias due to microcode bit matching criteria
			case 0x74:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_zero);

				(temp_imm8s < 0) ? Logging_LogChannel("JE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x65: // undocumented alias due to microcode bit matching criteria
			case 0x75:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, !cpu_8086.flag_zero);

				(temp_imm8s < 0) ? Logging_LogChannel("JNE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JNE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x66: // undocumented alias due to microcode bit matching criteria
			case 0x76:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_zero || cpu_8086.flag_carry);

				(temp_imm8s < 0) ? Logging_LogChannel("JBE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JBE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x67: // undocumented alias due to microcode bit matching criteria
			case 0x77:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_zero || cpu_8086.flag_carry);

				(temp_imm8s < 0) ? Logging_LogChannel("JA -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JA +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x68: // undocumented alias due to microcode bit matching criteria
			case 0x78:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_sign);

				(temp_imm8s < 0) ? Logging_LogChannel("JS -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JS +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x69: // undocumented alias due to microcode bit matching criteria
			case 0x79:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, !cpu_8086.flag_sign);

				(temp_imm8s < 0) ? Logging_LogChannel("JNS -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JNS +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x6A: // undocumented alias due to microcode bit matching criteria
			case 0x7A:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_parity);

				(temp_imm8s < 0) ? Logging_LogChannel("JPE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JPE +%02Xh", LogChannel_Debug, temp_imm8s);
				cpu_8086.IP++;
				break;
			case 0x6B: // undocumented alias due to microcode bit matching criteria
			case 0x7B:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, !cpu_8086.flag_parity);

				(temp_imm8s < 0) ? Logging_LogChannel("JPO -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JPO +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x6C: // undocumented alias due to microcode bit matching criteria
			case 0x7C:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_sign != cpu_8086.flag_overflow);

				(temp_imm8s < 0) ? Logging_LogChannel("JL -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JL +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x6D: // undocumented alias due to microcode bit matching criteria
			case 0x7D:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_sign == cpu_8086.flag_overflow);

				(temp_imm8s < 0) ? Logging_LogChannel("JGE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JGE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x6E: // undocumented alias due to microcode bit matching criteria
			case 0x7E:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, cpu_8086.flag_zero
					|| (cpu_8086.flag_parity != cpu_8086.flag_overflow));

				(temp_imm8s < 0) ? Logging_LogChannel("JLE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JLE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x6F: // undocumented alias due to microcode bit matching criteria
			case 0x7F:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC);
				i8086_JumpConditional(temp_imm8s, !cpu_8086.flag_zero
					&& (cpu_8086.flag_parity == cpu_8086.flag_overflow));

				(temp_imm8s < 0) ? Logging_LogChannel("JG -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JG +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0x80: // grp1
			case 0x81:
			case 0x82:
			case 0x83:
				i8086_Grp1(next_opcode);
				break;
			case 0x88:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				*modrm_info.final_offset = *modrm_info.reg_ptr8;
				
				cpu_8086.IP += 2; // 1 modrm byte
#if X86_DEBUG
				Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x89:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				*modrm_info.final_offset = *modrm_info.reg_ptr16;

				cpu_8086.IP += 2; // 1 modrm byte
#if X86_DEBUG
				Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x8A:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				*modrm_info.reg_ptr8 = &modrm_info.final_offset;

				cpu_8086.IP += 2; // 1 modrm byte
#if X86_DEBUG
				Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x8B:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				*modrm_info.reg_ptr16 = *modrm_info.final_offset;

				cpu_8086.IP += 2; // 1 modrm byte
#if X86_DEBUG
				Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x8C:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				*modrm_info.final_offset = *modrm_info.reg_ptr16;

				cpu_8086.IP += 2; // 1 modrm byte
#if X86_DEBUG
				Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x8E:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				*modrm_info.reg_ptr16 = *modrm_info.final_offset;

				cpu_8086.IP += 2; // 1 modrm byte
#if X86_DEBUG
				Logging_LogChannel("MOV %s", LogChannel_Debug, modrm_info.disasm);
#endif
				break;
			case 0x90:
				// lol
				// this is actually XCHG AX, AX
				// does anything depend on this behaviour?
				Logging_LogChannel("NOP", LogChannel_Debug);
				cpu_8086.IP++;
				break;
			case 0x91:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.CX;
				cpu_8086.CX = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG CX, AX", LogChannel_Debug);
				break;
			case 0x92:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.DX;
				cpu_8086.DX = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG DX, AX", LogChannel_Debug);
				break;
			case 0x93:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.BX;
				cpu_8086.BX = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG BX, AX", LogChannel_Debug);
				break;
			case 0x94:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.SP;
				cpu_8086.SP = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG SP, AX", LogChannel_Debug);
				break;
			case 0x95:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.BP;
				cpu_8086.BP = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG BP, AX", LogChannel_Debug);
				break;
			case 0x96:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.SI;
				cpu_8086.SI = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG SI, AX", LogChannel_Debug);
				break;
			case 0x97:
				temp_imm16u_01 = cpu_8086.AX;
				cpu_8086.AX = cpu_8086.DI;
				cpu_8086.DI = temp_imm16u_01;
				cpu_8086.IP++;
				Logging_LogChannel("XCHG DI, AX", LogChannel_Debug);
				break;
			case 0x9A:
				i8086_Push(cpu_8086.IP + 4); // offset of return address after call
				i8086_Push(cpu_8086.CS);     // and the segment

				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				temp_imm16u_02 = i8086_ReadU16(cpu_8086._PC += 2);

				cpu_8086.IP = temp_imm16u_01;
				cpu_8086.CS = temp_imm16u_02;

				Logging_LogChannel("CALL FAR %04Xh:%04Xh", LogChannel_Debug, temp_imm16u_01, temp_imm16u_02);
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
				break;
				// a0-a3: bizarre special case instructions
			case 0xA0:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				i8086_MoveSegOff8(temp_imm8u, false);
				break;
			case 0xA1:
				temp_imm16u_01 = i8086_ReadU8(cpu_8086._PC);
				i8086_MoveSegOff16(temp_imm8u, false);
				break;
				// a0-a3: bizarre special case instructions
			case 0xA2:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				i8086_MoveSegOff8(temp_imm8u, true);
				break;
			case 0xA3:
				temp_imm16u_01 = i8086_ReadU8(cpu_8086._PC);
				i8086_MoveSegOff16(temp_imm8u, true);
				break;
				// b0-bf: 16-bit move immediate instructions
			case 0xB0:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.AL = temp_imm8u;
				Logging_LogChannel("MOV AL, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB1:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.CL = temp_imm8u;
				Logging_LogChannel("MOV CL, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB2:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.DL = temp_imm8u;
				Logging_LogChannel("MOV DL, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB3:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.BL = temp_imm8u;
				Logging_LogChannel("MOV BL, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB4:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.AH = temp_imm8u;
				Logging_LogChannel("MOV AH, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB5:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.CH = temp_imm8u;
				Logging_LogChannel("MOV CH, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB6:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.DH = temp_imm8u;
				Logging_LogChannel("MOV DH, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB7:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086.BH = temp_imm8u;
				Logging_LogChannel("MOV BH, %02Xh", LogChannel_Debug, temp_imm8u);
				cpu_8086.IP += 2; // 1 modrm byte
				break;
			case 0xB8:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.AX = temp_imm16u_01;
				Logging_LogChannel("MOV AX, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xB9:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.CX = temp_imm16u_01;
				Logging_LogChannel("MOV CX, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xBA:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.DX = temp_imm16u_01;
				Logging_LogChannel("MOV DX, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xBB:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.BX = temp_imm16u_01;
				Logging_LogChannel("MOV BX, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xBC:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.SP = temp_imm16u_01;
				Logging_LogChannel("MOV SP, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xBD:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.BP = temp_imm16u_01;
				Logging_LogChannel("MOV BP, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xBE:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.SI = temp_imm16u_01;
				Logging_LogChannel("MOV SI, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xBF:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);
				cpu_8086.DI = temp_imm16u_01;
				Logging_LogChannel("MOV DI, %04Xh", LogChannel_Debug, temp_imm16u_01);
				cpu_8086.IP += 3;
				break;
			case 0xC0: // undocumented alias
			case 0xC2:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);

				// pull return address
				uint16_t ret_address = i8086_Pop();
				
				// operand2 is number of bytes to free from stack
				// for parameters etc..
				for (int i = 0; i < temp_imm16u_01; i += 2)
				{
					// throw away
					i8086_Pop();
				}

				cpu_8086.IP = ret_address;
				Logging_LogChannel("RETN CS:%04Xh", LogChannel_Debug, temp_imm16u_01);
				break;
			case 0xC1:
			case 0xC3:
				// pull return address for IP
				cpu_8086.IP = i8086_Pop();
				Logging_LogChannel("NEAR RET to %04Xh:%04Xh", LogChannel_Debug, cpu_8086.CS, cpu_8086.IP);
				break;
			case 0xC6:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				// we don't need the previous temp_imm8u value
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);

				*modrm_info.reg_ptr8 = temp_imm8u;
				cpu_8086.IP += 3;
#if X86_DEBUG
				Logging_LogChannel("MOV %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm8u);
#endif
				break;
			case 0xC7:
				temp_imm8u = i8086_ReadU8(cpu_8086._PC);
				cpu_8086._PC++;
				modrm_info = i8086_ModRM(next_opcode, temp_imm8u);

				// we don't need the previous temp_imm8u value
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);

				*modrm_info.reg_ptr16 = temp_imm16u_01;
				cpu_8086.IP += 4;
#if X86_DEBUG
				Logging_LogChannel("MOV %s, %04Xh", LogChannel_Debug, modrm_info.disasm, temp_imm16u_01);
#endif
				break;
			case 0xC8: // undocumented alias
			case 0xCA:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC);

				// pull return address
				uint16_t ret_address_off = i8086_Pop();
				uint16_t ret_address_seg = i8086_Pop();

				// THIS IS DONE AFTER
 
				// operand2 is number of bytes to free from stack
				// for parameters etc..
				for (int i = 0; i < temp_imm16u_01; i += 2)
				{
					// throw away
					i8086_Pop();
				}

				cpu_8086.IP = ret_address_off;
				cpu_8086.CS = ret_address_seg;
				Logging_LogChannel("FAR RETN %04Xh (to %04Xh:%04Xh)", LogChannel_Debug, temp_imm16u_01, cpu_8086.CS, cpu_8086.IP);
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
				interrupt_num = i8086_ReadU8(cpu_8086.IP++); //increment IP and read 

				Logging_LogChannel("The application called an interrupt service routine.\n\n"
					"INT %02Xh AX=%04Xh BX=%04Xh CX=%04Xh DX=%04Xh\n"
					"CS=%04Xh IP=%04Xh (Physical address of PC=%05X) DS=%04Xh ES=%04Xh SS=%04Xh\n"
					"BP=%04Xh DI=%04Xh SI=%04Xh", LogChannel_Debug, interrupt_num,
					cpu_8086.AX, cpu_8086.BX, cpu_8086.CX, cpu_8086.DX, cpu_8086.CS, cpu_8086.DS, cpu_8086.ES, cpu_8086.SS,
					cpu_8086.CS, cpu_8086.IP, cpu_8086._PC, cpu_8086.DS, cpu_8086.ES, cpu_8086.SS, cpu_8086.BP, cpu_8086.DI, cpu_8086.ES);
				cpu_8086.IP++;
				break;
			case 0xE0:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(temp_imm8s, cpu_8086.CX > 0);

				(temp_imm8s < 0) ? Logging_LogChannel("LOOP -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("LOOP +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0xE1:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(temp_imm8s, cpu_8086.CX > 0 && cpu_8086.flag_zero);

				(temp_imm8s < 0) ? Logging_LogChannel("LOOPE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("LOOPE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0xE2:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(temp_imm8s, cpu_8086.CX > 0 && !cpu_8086.flag_zero);

				(temp_imm8s < 0) ? Logging_LogChannel("LOOPNE -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("LOOPNE +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0xE3:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC++);
				i8086_Loop(temp_imm8s, cpu_8086.CX == 0);

				(temp_imm8s < 0) ? Logging_LogChannel("JCXZ -%02Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JCXZ +%02Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0xE8:
				i8086_Push(cpu_8086._PC + 4); // return address after call

				temp_imm16s_01 = i8086_ReadS16(cpu_8086._PC++);

				cpu_8086.IP += temp_imm16s_01;
				(temp_imm8s < 0) ? Logging_LogChannel("CALL NEAR -%04Xh", LogChannel_Debug, temp_imm16s_01) : Logging_LogChannel("CALL +%04Xh", LogChannel_Debug, temp_imm16s_01);
				break;
			case 0xE9:
				temp_imm16s_01 = i8086_ReadS16(cpu_8086._PC++);

				cpu_8086.IP += temp_imm16s_01;
				(temp_imm8s < 0) ? Logging_LogChannel("JMP NEAR -%04Xh", LogChannel_Debug, temp_imm8s) : Logging_LogChannel("JMP +%04Xh", LogChannel_Debug, temp_imm8s);
				break;
			case 0xEA:
				temp_imm16u_01 = i8086_ReadU16(cpu_8086._PC++);
				temp_imm16u_02 = i8086_ReadU16(cpu_8086._PC += 2);

				cpu_8086.IP = temp_imm16u_01;
				cpu_8086.CS = temp_imm16u_02;

				Logging_LogChannel("JMP FAR %04Xh:%04Xh", LogChannel_Debug, temp_imm16u_01, temp_imm16u_02);

				break;
			case 0xEB:
				temp_imm8s = i8086_ReadS8(cpu_8086._PC++);

				(temp_imm8s < 0) ? Logging_LogChannel("JMP NEAR -%04Xh", LogChannel_Debug, temp_imm16s_01) : Logging_LogChannel("JMP +%04Xh", LogChannel_Debug, temp_imm16s_01);

				cpu_8086.IP += temp_imm8s;
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
				Logging_LogChannel("Unimplemented 8086 opcode 0x%X @ %04Xh:%04Xh", LogChannel_Error, next_opcode, cpu_8086.CS, cpu_8086.IP);
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