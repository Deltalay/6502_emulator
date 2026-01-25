/*
 * 6502 Emulator
 * Copyright (C) 2026 Deltalay
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
// 8bit;
typedef unsigned char BYTE;
// 16bit;
typedef unsigned short WORD;
typedef char SBYTE;
typedef short SWORD;
// Memory for 6502 (64KB)
BYTE memory[1 * 64 * 1024];
// Not a good idea but why not
typedef struct
{
  unsigned N : 1;
  unsigned V : 1;
  unsigned U : 1;
  unsigned B : 1;
  unsigned D : 1;
  unsigned I : 1;
  unsigned Z : 1;
  unsigned C : 1;
} Status;
typedef struct
{
  BYTE A;
  BYTE X;
  BYTE Y;
  WORD PC;
  BYTE S;
  Status P;
} CPU;
BYTE mem_read(WORD address)
{
  return memory[address];
}
void mem_write(WORD address, BYTE value)
{
  memory[address] = value;
}
void cpu_reset(CPU *cpu)
{
  cpu->S = 0xFD;
  BYTE low = memory[0xFFFC];
  BYTE high = memory[0xFFFD];
  cpu->PC = ((WORD)high << 8) | low;
  cpu->A = cpu->X = cpu->Y = 0;
  cpu->P.U = 1;
}
// TODO: Implement more op_code.
// Also we do not care about cycle right now.
// TODO: LOAD: LDA, LDX, LDY, STA, STX
#define LDA_IMMEDIATE 0xA9
#define LDA_ZEROPAGE 0xA5
#define LDA_ZEROPAGE_X 0xB5
#define LDA_ABSOLUTE 0xAD
#define LDA_ABSOLUTE_X 0xBD
#define LDA_ABSOLUTE_Y 0xB9
#define LDA_INDIRECT_X 0xA1
#define LDA_INDIRECT_Y 0xB1
#define LDX_IMMEDIATE 0xA2
#define LDX_ZEROPAGE 0xA6
#define LDX_ZEROPAGE_Y 0xB6
#define LDX_ABSOLUTE 0xAE
#define LDX_ABSOLUTE_Y 0xBE
#define LDY_IMMEDIATE 0xA0
#define LDY_ZEROPAGE 0xA4
#define LDY_ZEROPAGE_X 0xB4
#define LDY_ABSOLUTE 0xAC
#define LDY_ABSOLUTE_X 0xBC
#define STA_ZEROPAGE 0x85
#define STA_ZEROPAGE_X 0x95
#define STA_ABSOLUTE 0x8D
#define STA_ABSOLUTE_X 0x9D
#define STA_ABSOLUTE_Y 0x99
#define STA_INDIRECT_X 0x81
#define STA_INDIRECT_Y 0x91
#define STX_ZEROPAGE 0x86
#define STX_ZEROPAGE_Y 0x96
#define STX_ABSOLUTE 0x8E
#define STY_ZEROPAGE 0x84
#define STY_ZEROPAGE_X 0x94
#define STY_ABSOLUTE 0x8C
#define TAX 0xAA
#define TAY 0xA8
#define TSX 0xBA
#define TXA 0x8A
#define TXS 0x9A
#define TYA 0x98
#define SEC 0x38
#define SED 0xF8
#define SEI 0x78
#define BRK 0x00
#define NOP 0xEA
#define INX 0xE8
#define INY 0xC8
#define CLC 0x18
#define CLD 0xD8
#define CLI 0x58
#define CLV 0xB8
#define BCC 0x90
#define BCS 0xB0
#define BEQ 0xF0
#define BMI 0x30
#define BNE 0xD0
#define BPL 0x10
#define BVC 0x50
#define BVS 0x70
#define INC_ZEROPAGE 0xE6
#define INC_ZEROPAGE_X 0xF6
#define INC_ABSOLUTE 0xEE
#define INC_ABSOLUTE_X 0xFE
#define DEC_ZEROPAGE 0xC6
#define DEC_ZEROPAGE_X 0xD6
#define DEC_ABSOLUTE 0xCE
#define DEC_ABSOLUTE_X 0xDE
#define DEX 0xCA
#define DEY 0x88
#define ADC_IMMEDIATE 0x69
#define ADC_ZEROPAGE 0x65
#define ADC_ZEROPAGE_X 0x75
#define ADC_ABSOLUTE 0x6D
#define ADC_ABSOLUTE_X 0x7D
#define ADC_ABSOLUTE_Y 0x79
#define ADC_INDIRECT_X 0x61
#define ADC_INDIRECT_Y 0x71
void setZN(CPU *cpu, BYTE val)
{
  cpu->P.Z = (val == 0);
  cpu->P.N = (val & 0x80) != 0;
  cpu->P.U = 1;
}
void setZVN(CPU *cpu, BYTE val )
{
  cpu->P.Z = (cpu->A == 0);
  cpu->P.N = (val & 0x80) != 0;
  cpu->P.U = 1;
}
void execute(CPU *cpu)
{
  BYTE op_code = mem_read(cpu->PC++);
  switch (op_code)
  {
  case LDA_IMMEDIATE:
  {
    cpu->A = mem_read(cpu->PC++);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    cpu->A = mem_read(addr);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_ZEROPAGE_X:
  {
    BYTE base = mem_read(cpu->PC++);
    BYTE addr = (BYTE)(base + cpu->X) & 0xFF;
    cpu->A = mem_read(addr);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    cpu->A = mem_read(addr);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_ABSOLUTE_X:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    addr = (addr + (WORD)cpu->X) & 0xFFFF;
    cpu->A = mem_read(addr);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_ABSOLUTE_Y:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    addr = (addr + (WORD)cpu->Y) & 0xFFFF;
    cpu->A = mem_read(addr);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_INDIRECT_X:
  {
    BYTE ptr = mem_read(cpu->PC++);
    BYTE addr_ptr = (BYTE)(ptr + cpu->X);
    BYTE first_addr = mem_read(addr_ptr);
    BYTE second_addr = mem_read((addr_ptr + 0x01) & 0xFF);
    WORD addr = (second_addr << 8) | first_addr;
    cpu->A = mem_read(addr);
    setZN(cpu, cpu->A);
    break;
  }
  case LDA_INDIRECT_Y:
  {
    BYTE addr_ptr = mem_read(cpu->PC++);
    BYTE first_addr = mem_read(addr_ptr);
    BYTE second_addr = mem_read((addr_ptr + 0x01) & 0xFF);
    WORD addr = (second_addr << 8) | first_addr;
    cpu->A = mem_read((addr + cpu->Y) & 0xFFFF);
    setZN(cpu, cpu->A);
    break;
  }
  case LDX_IMMEDIATE:
  {
    cpu->X = mem_read(cpu->PC++);
    setZN(cpu, cpu->X);
    break;
  }
  case LDX_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    cpu->X = mem_read(addr);
    setZN(cpu, cpu->X);
    break;
  }
  case LDX_ZEROPAGE_Y:
  {
    BYTE base = mem_read(cpu->PC++);
    BYTE addr = (BYTE)(base + cpu->Y) & 0xFF;
    cpu->X = mem_read(addr);
    setZN(cpu, cpu->X);
    break;
  }
  case LDX_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    cpu->X = mem_read(addr);
    setZN(cpu, cpu->X);
    break;
  }
  case LDX_ABSOLUTE_Y:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->Y) & 0xFFFF;
    cpu->X = mem_read(addr);
    setZN(cpu, cpu->X);
    break;
  }
  case LDY_IMMEDIATE:
  {
    cpu->Y = mem_read(cpu->PC++);
    setZN(cpu, cpu->Y);
    break;
  }
  case LDY_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    cpu->Y = mem_read(addr);
    setZN(cpu, cpu->Y);
    break;
  }
  case LDY_ZEROPAGE_X:
  {
    BYTE base = mem_read(cpu->PC++);
    BYTE addr = (BYTE)(base + cpu->X) & 0xFF;
    cpu->Y = mem_read(addr);
    setZN(cpu, cpu->Y);
    break;
  }
  case LDY_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    cpu->Y = mem_read(addr);
    setZN(cpu, cpu->Y);
    break;
  }
  case LDY_ABSOLUTE_X:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->X) & 0xFFFF;
    cpu->Y = mem_read(addr);
    setZN(cpu, cpu->Y);
    break;
  }
  case STA_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    BYTE value_A = cpu->A;
    mem_write((WORD)addr, value_A);
    break;
  }
  case STA_ZEROPAGE_X:
  {
    BYTE addr = (mem_read(cpu->PC++) + cpu->X) & 0xFF;
    mem_write((WORD)addr, cpu->A);
    break;
  }
  case STA_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    mem_write(addr, cpu->A);
    break;
  }
  case STA_ABSOLUTE_X:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->X) & 0xFFFF;
    mem_write(addr, cpu->A);
    break;
  }
  case STA_ABSOLUTE_Y:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->Y) & 0xFFFF;
    mem_write(addr, cpu->A);
    break;
  }
  case STA_INDIRECT_X:
  {
    BYTE ptr = mem_read(cpu->PC++);
    BYTE addr_ptr = (ptr + cpu->X) & 0xFF;
    BYTE first_addr = mem_read(addr_ptr);
    BYTE second_addr = mem_read((addr_ptr + 0x01) & 0xFF);
    WORD addr = (second_addr << 8) | first_addr;
    mem_write(addr, cpu->A);
    break;
  }
  case STA_INDIRECT_Y:
  {
    BYTE addr_ptr = mem_read(cpu->PC++);
    BYTE first_addr = mem_read(addr_ptr);
    BYTE second_addr = mem_read((addr_ptr + 0x01) & 0xFF);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->Y) & 0xFFFF;
    mem_write(addr, cpu->A);
    break;
  }
  case STX_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    BYTE value_X = cpu->X;
    mem_write((WORD)addr, value_X);
    break;
  }
  case STX_ZEROPAGE_Y:
  {
    BYTE addr = (mem_read(cpu->PC++) + cpu->Y) & 0xFF;
    mem_write((WORD)addr, cpu->X);
    break;
  }
  case STX_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    mem_write(addr, cpu->X);
    break;
  }
  case STY_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    mem_write((WORD)addr, cpu->Y);
    break;
  }
  case STY_ZEROPAGE_X:
  {
    BYTE addr = (mem_read(cpu->PC++) + cpu->X) & 0xFF;
    mem_write((WORD)addr, cpu->Y);
    break;
  }
  case STY_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    mem_write(addr, cpu->Y);
    break;
  }
  case INC_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    BYTE val = mem_read(addr);
    val = (val + 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case INC_ZEROPAGE_X:
  {
    BYTE addr = mem_read(cpu->PC++);
    addr = (addr + cpu->X) & 0xFF;
    BYTE val = mem_read(addr);
    val = (val + 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case INC_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    BYTE val = mem_read(addr);
    val = (val + 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case INC_ABSOLUTE_X:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->X) & 0xFFFF;
    BYTE val = mem_read(addr);
    val = (val + 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case BCC:
  {
   SBYTE offset = mem_read(cpu->PC++);
   if (cpu->P.C == 0) {
       cpu->PC = (cpu->PC + offset) & 0xFFFF;
   }
   break;
  }
  case BCS:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.C == 1)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;
      }
      break;
  }
  case BEQ:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.Z == 1)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;
      }
      break;
  }
  case BMI:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.N == 1)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;
      }
      break;
  }
  case BNE:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.Z == 0)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;
      }
      break;
  }
  case BPL:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.N == 0)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;
      }
      break;
  }
  case BVC:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.V == 0)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;

      }
      break;
  }
  case BVS:
  {
      SBYTE offset = mem_read(cpu->PC++);
      if (cpu->P.V == 1)
      {
          cpu->PC = (cpu->PC + offset) & 0xFFFF;
      }
      break;
  }
  case INX:
  {
      cpu->X = (cpu->X + 1) & 0xFF;
      setZN(cpu, cpu->X);
      break;
  }
  case INY:
  {
      cpu->Y = (cpu->Y + 1) & 0xFF;
      setZN(cpu, cpu->Y);
      break;
  }
  case DEC_ZEROPAGE:
  {
    BYTE addr = mem_read(cpu->PC++);
    BYTE val = mem_read(addr);
    val = (val - 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case DEC_ZEROPAGE_X:
  {
    BYTE addr = mem_read(cpu->PC++);
    addr = (addr + cpu->X) & 0xFF;
    BYTE val = mem_read(addr);
    val = (val - 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case DEC_ABSOLUTE:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (second_addr << 8) | first_addr;
    BYTE val = mem_read(addr);
    val = (val - 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case DEC_ABSOLUTE_X:
  {
    BYTE first_addr = mem_read(cpu->PC++);
    BYTE second_addr = mem_read(cpu->PC++);
    WORD addr = (((second_addr << 8) | first_addr) + cpu->X) & 0xFFFF;
    BYTE val = mem_read(addr);
    val = (val - 1) & 0xFF;
    mem_write(addr, val);
    setZN(cpu, val);
    break;
  }
  case DEX:
  {
      cpu->X = (cpu->X - 1) & 0xFF;
      setZN(cpu, cpu->X);
      break;
  }
  case DEY:
  {
      cpu->Y = (cpu->Y - 1) & 0xFF;
      setZN(cpu, cpu->Y);
      break;
  }
  case TAX:
  {
    cpu->X = cpu->A;
    setZN(cpu, cpu->X);
    break;
  }
  case TAY:
  {
    cpu->Y = cpu->A;
    setZN(cpu, cpu->Y);
    break;
  }
  case TSX:
  {
    cpu->X = cpu->S;
    setZN(cpu, cpu->X);
    break;
  }
  case TXA:
  {
    cpu->A = cpu->X;
    setZN(cpu, cpu->A);
    break;
  }
  case TXS:
  {
    cpu->S = cpu->X;
    break;
  }
  case TYA:
  {
    cpu->A = cpu->Y;
    setZN(cpu, cpu->Y);
    break;
  }
  case CLC:
  {
    cpu->P.C = 0;
    break;
  }
  case SEC:
  {
    cpu->P.C = 1;
    break;
  }
  case SED:
  {
    cpu->P.D = 1;
    break;
  }
  case SEI:
  {
    cpu->P.I = 1;
    break;
  }
  case CLD:
  {
      cpu->P.D = 0;
      break;
  }
  case CLI:
  {
      cpu->P.I = 0;
      break;
  }
  case CLV:
  {
      cpu->P.V = 0;
      break;
  }
  case NOP:
  {
    printf("NOP\n");
    break;
  }
  case BRK:
    printf("BRK\n");
    exit(0);
    break;
  default:
    printf("Opcode 0x%02x at PC=0x%04x\n", op_code, cpu->PC - 1);
    exit(1);
    break;
  }
}
int main()
{
  CPU cpu;

  mem_write(0xFFFC, 0x00);
  mem_write(0xFFFD, 0x80);
  cpu_reset(&cpu);
  printf("CPU reset complete. PC = 0x%04X, S = 0x%02X, U = %d, X = 0x%02X\n",
         cpu.PC, cpu.S, cpu.P.U, cpu.X);
  mem_write(0x8000, LDA_IMMEDIATE);
  mem_write(0x8001, 0x10);
  mem_write(0x8002, LDX_IMMEDIATE);
  mem_write(0x8003, 0x03);
  mem_write(0x8004, LDY_IMMEDIATE);
  mem_write(0x8005, 0x04);
  mem_write(0x8006, STA_ZEROPAGE);
  mem_write(0x8007, 0x20);
  mem_write(0x8008, STX_ZEROPAGE);
  mem_write(0x8009, 0x21);
  mem_write(0x800A, STY_ZEROPAGE);
  mem_write(0x800B, 0x22);
  mem_write(0x800C, LDA_ZEROPAGE_X);
  mem_write(0x800D, 0x1D); // 1D + X = 20
  mem_write(0x800E, LDX_ZEROPAGE_Y);
  mem_write(0x800F, 0x1E); // 1E + Y = 22
  mem_write(0x8010, LDY_ZEROPAGE_X);
  mem_write(0x8011, 0x1E); // 1E + X = 21
  mem_write(0x8012, LDA_ABSOLUTE);
  mem_write(0x8013, 0x00);
  mem_write(0x8014, 0x90);
  mem_write(0x8015, LDX_ABSOLUTE);
  mem_write(0x8016, 0x01);
  mem_write(0x8017, 0x90);
  mem_write(0x8018, LDY_ABSOLUTE);
  mem_write(0x8019, 0x02);
  mem_write(0x801A, 0x90);
  mem_write(0x801B, TAX);
  mem_write(0x801C, TAY);
  mem_write(0x801D, TXA);
  mem_write(0x801E, TYA);
  mem_write(0x801F, TSX);
  mem_write(0x8020, TXS);
  mem_write(0x8021, INC_ZEROPAGE);
  mem_write(0x8022, 0x30);
  mem_write(0x8023, INC_ZEROPAGE_X);
  mem_write(0x8024, 0x2D);
  mem_write(0x8025, INC_ABSOLUTE);
  mem_write(0x8026, 0x03);
  mem_write(0x8027, 0x90);
  mem_write(0x8028, INC_ABSOLUTE_X);
  mem_write(0x8029, 0x03);
  mem_write(0x802A, 0x90);
  mem_write(0x802B, CLC);
  mem_write(0x802C, SEC);
  mem_write(0x802D, SED);
  mem_write(0x802E, SEI);
  mem_write(0x802F, NOP);
  mem_write(0x8030, NOP);
  mem_write(0x8031, BRK);
  mem_write(0x0020, 0xAA);
  mem_write(0x0021, 0xBB);
  mem_write(0x0022, 0xCC);
  mem_write(0x0030, 0x01);     // INC zp
  mem_write(0x0030 + 3, 0x05); // INC zp,X (X=3)
  mem_write(0x9000, 0x11);
  mem_write(0x9001, 0x22);
  mem_write(0x9002, 0x33);
  mem_write(0x9003, 0x44);     // INC abs
  mem_write(0x9003 + 3, 0x66); // INC abs,X
  while (1)
  {
    execute(&cpu);
    printf("A=%02X X=%02X Y=%02X Z=%d N=%d PC=%04X\n",
           cpu.A, cpu.X, cpu.Y, cpu.P.Z, cpu.P.N, cpu.PC);
  }
  return 0;
}
