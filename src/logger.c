#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/logger.h"
#define UNOFFICIAL_OPCODES

static FILE *fp = NULL;

void init_log(void) {
  time_t now;
  struct tm *t;
  time(&now);
  t = localtime(&now);
#ifndef DEBUG
  char buffer[256];
  strftime(buffer, sizeof(buffer), "%c", t);
  strcat(buffer, ".log");
#else
  char *buffer = "m6502.log";
#endif
  printf("opening: %s\n", buffer);

  fp = fopen(buffer, "w");
  if (fp == NULL) {
    printf("Error could not open file: %s", strerror(errno));
  }
}

static inline uint16_t read_word_at(processor_t *processor, uint16_t address) {
  switch (address >> 14) {
  case 1:
    address &= ~0xffffc000;
    break;
  case 2:
    address &= ~0xffffdff8;
    break;
  case 3:
    address &= ~0xffff4000;
    break;
  }
  uint16_t value =
      (processor->memory[address] | (processor->memory[address + 1] << 8));
  return value;
}

static inline uint8_t read_byte_at(processor_t *processor, uint16_t address) {
  switch (address >> 14) {
  case 1:
    address &= ~0xffffc000; /* PRG RAM */
    break;
  case 2:
    address &= ~0xffffdff8; /* APU */
    break;
  case 3:
    address &= ~0xffff4000; /* PRG ROM */
    break;
  }
  uint8_t value = processor->memory[address++];
  return value;
}
/**
   @brief writes to a log file, if debug build it also prints to stdio
   this will probably be expanded on, to add the instructions

   this is a terribly written function which will probably never be optimized
   @param reg cpu registers
 */
void log_cpu(processor_t processor) {
  typedef struct {
    const char *const format;
    uint8_t len;
  } opcode_t;

  /* formats from radare source
   * https://github.com/radare/radare2/blob/master/libr/asm/arch/6502/6502dis.c
   */
  static opcode_t ops[] = {
      [0x01] = {"ORA ($%02X, X)", 2},
      [0x05] = {"ORA $%02X", 2},
      [0x06] = {"ASL $%02X", 2},
      [0x08] = {"PHP ", 1},
      [0x09] = {"ORA #$%02X", 2},
      [0x0a] = {"ASL A", 1},
      [0x0d] = {"ORA $%04X", 3},
      [0x0e] = {"ASL $%04X", 3},
      [0x10] = {"BPL $%02X", 2},
      [0x11] = {"ORA ($%02X), Y", 2},
      [0x15] = {"ORA $%02X, X", 2},
      [0x16] = {"ASL $%02X, X", 2},
      [0x18] = {"CLC ", 1},
      [0x19] = {"ORA $%04X, Y", 3},
      [0x1d] = {"ORA $%04X, X", 3},
      [0x1e] = {"ASL $%04X, X", 3},
      [0x20] = {"JSR $%04X", 3},
      [0x21] = {"AND ($%02X, X)", 2},
      [0x24] = {"BIT $%02X", 2},
      [0x25] = {"AND $%02X", 2},
      [0x26] = {"ROL $%02X", 2},
      [0x28] = {"PLP ", 1},
      [0x29] = {"AND #$%02X", 2},
      [0x2a] = {"ROL A", 1},
      [0x2c] = {"BIT $%04X", 3},
      [0x2d] = {"AND $%04X", 3},
      [0x2e] = {"ROL $%04X", 3},
      [0x30] = {"BMI $%02X", 2},
      [0x31] = {"AND ($%02X), Y", 2},
      [0x35] = {"AND $%02X, X", 2},
      [0x36] = {"ROL $%02X, X", 2},
      [0x38] = {"SEC ", 1},
      [0x39] = {"AND $%04X, Y", 3},
      [0x3d] = {"AND $%04X, X", 3},
      [0x3e] = {"ROL $%04X, X", 3},
      [0x40] = {"RTI ", 1},
      [0x41] = {"EOR ($%02X, X)", 2},
      [0x45] = {"EOR $%02X", 2},
      [0x46] = {"LSR $%02X", 2},
      [0x48] = {"PHA ", 1},
      [0x49] = {"EOR #$%02X", 2},
      [0x4a] = {"LSR A", 1},
      [0x4c] = {"JMP $%04X", 3},
      [0x4d] = {"EOR $%04X", 3},
      [0x4e] = {"LSR $%04X", 3},
      [0x50] = {"BVC $%02X", 2},
      [0x51] = {"EOR ($%02X), Y", 2},
      [0x55] = {"EOR $%02X, X", 2},
      [0x56] = {"LSR $%02X, X", 2},
      [0x58] = {"CLI ", 1},
      [0x59] = {"EOR $%04X, Y", 3},
      [0x5d] = {"EOR $%04X, X", 3},
      [0x5e] = {"LSR $%04X, X", 3},
      [0x60] = {"RTS ", 1},
      [0x61] = {"ADC ($%02X, X)", 2},
      [0x65] = {"ADC $%02X", 2},
      [0x66] = {"ROR $%02X", 2},
      [0x68] = {"PLA ", 1},
      [0x69] = {"ADC #$%02X", 2},
      [0x6a] = {"ROR A", 1},
      [0x6c] = {"JMP ($%04X)", 3},
      [0x6d] = {"ADC $%04X", 3},
      [0x6e] = {"ROR $%04X", 3},
      [0x70] = {"BVS $%02X", 2},
      [0x71] = {"ADC ($%02X), Y", 2},
      [0x75] = {"ADC $%02X, X", 2},
      [0x76] = {"ROR $%02X, X", 2},
      [0x78] = {"SEI ", 1},
      [0x79] = {"ADC $%04X, Y", 3},
      [0x7d] = {"ADC $%04X, X", 3},
      [0x7e] = {"ROR $%04X, X", 3},
      [0x81] = {"STA ($%02X, X)", 2},
      [0x84] = {"STY $%02X", 2},
      [0x85] = {"STA $%02X", 2},
      [0x86] = {"STX $%02X", 2},
      [0x88] = {"DEY ", 1},
      [0x8a] = {"TXA ", 1},
      [0x8c] = {"STY $%04X", 3},
      [0x8d] = {"STA $%04X", 3},
      [0x8e] = {"STX $%04X", 3},
      [0x90] = {"BCC $%02X", 2},
      [0x91] = {"STA ($%02X), Y", 2},
      [0x94] = {"STY $%02X, X", 2},
      [0x95] = {"STA $%02X, X", 2},
      [0x96] = {"STX $%02X, Y", 2},
      [0x98] = {"TYA ", 1},
      [0x99] = {"STA $%04X, Y", 3},
      [0x9a] = {"TXS ", 1},
      [0x9d] = {"STA $%04X, X", 3},
      [0xa0] = {"LDY #$%02X", 2},
      [0xa1] = {"LDA ($%02X, X)", 2},
      [0xa2] = {"LDX #$%02X", 2},
      [0xa4] = {"LDY $%02X", 2},
      [0xa5] = {"LDA $%02X", 2},
      [0xa6] = {"LDX $%02X", 2},
      [0xa8] = {"TAY ", 1},
      [0xa9] = {"LDA #$%02X", 2},
      [0xaa] = {"TAX ", 1},
      [0xac] = {"LDY $%04X", 3},
      [0xad] = {"LDA $%04X", 3},
      [0xae] = {"LDX $%04X", 3},
      [0xb0] = {"BCS $%02X", 2},
      [0xb1] = {"LDA ($%02X), Y", 2},
      [0xb4] = {"LDY $%02X, X", 2},
      [0xb5] = {"LDA $%02X, X", 2},
      [0xb6] = {"LDX $%02X, Y", 2},
      [0xb8] = {"CLV ", 1},
      [0xb9] = {"LDA $%04X, Y", 3},
      [0xba] = {"TSX ", 1},
      [0xbc] = {"LDY $%04X, X", 3},
      [0xbd] = {"LDA $%04X, X", 3},
      [0xbe] = {"LDX $%04X, Y", 3},
      [0xc0] = {"CPY #$%02X", 2},
      [0xc1] = {"CMP ($%02X, X)", 2},
      [0xc4] = {"CPY $%02X", 2},
      [0xc5] = {"CMP $%02X", 2},
      [0xc6] = {"DEC $%02X", 2},
      [0xc8] = {"INY ", 1},
      [0xc9] = {"CMP #$%02X", 2},
      [0xca] = {"DEX ", 1},
      [0xcc] = {"CPY $%04X", 3},
      [0xcd] = {"CMP $%04X", 3},
      [0xce] = {"DEC $%04X", 3},
      [0xd0] = {"BNE $%02X", 2},
      [0xd1] = {"CMP ($%02X), Y", 2},
      [0xd5] = {"CMP $%02X, X", 2},
      [0xd6] = {"DEC $%02X, X", 2},
      [0xd8] = {"CLD ", 1},
      [0xd9] = {"CMP $%04X, Y", 3},
      [0xdd] = {"CMP $%04X, X", 3},
      [0xde] = {"DEC $%04X, X", 3},
      [0xe0] = {"CPX #$%02X", 2},
      [0xe4] = {"CPX $%02X", 2},
      [0xe6] = {"INC $%02X", 2},
      [0xe8] = {"INX ", 1},
      [0xec] = {"CPX $%04X", 3},
      [0xee] = {"INC $%04X", 3},
      [0xf0] = {"BEQ $%02X", 2},
      [0xf6] = {"INC $%02X, X", 2},
      [0xf8] = {"SED ", 1},
      [0xfe] = {"INC $%04X, X", 3},
      [0x00] = {"BRK", 1},
      [0xea] = {"NOP", 1},
      [0xe9] = {"SBC #$%02X", 2},
      [0xe5] = {"SBC $%02X", 2},
      [0xf5] = {"SBC $%02X,X", 2},
      [0xed] = {"SBC $%04X", 3},
      [0xfd] = {"SBC $%04X,X", 3},
      [0xf9] = {"SBC $%04X,Y", 3},
      [0xe1] = {"SBC ($%02X,X)", 2},
      [0xf1] = {"SBC ($%02X),Y", 2},
      [0xeb] = {"SBC #$%02X", 2},

#ifdef UNOFFICIAL_OPCODES
      [0x0b] = {"ANC #$%02X", 2},
      [0x2b] = {"ANC #$%02X", 2},
      [0x8b] = {"ANE #$%02X", 2},
      [0x6b] = {"ARR #$%02X", 2},
      [0x4b] = {"ASR #$%02X", 2},
      [0xc7] = {"DCP $%02X", 2},
      [0xd7] = {"DCP $%02X,X", 2},
      [0xcf] = {"DCP $%04X", 3},
      [0xdf] = {"DCP $%04X,X", 3},
      [0xdb] = {"DCP $%04X,Y", 3},
      [0xc3] = {"DCP ($%02X,X)", 2},
      [0xd3] = {"DCP ($%02X),Y", 2},
      [0xe7] = {"ISB $%02X", 2},
      [0xf7] = {"ISB $%02X,X", 2},
      [0xef] = {"ISB $%04X", 3},
      [0xff] = {"ISB $%04X,X", 3},
      [0xfb] = {"ISB $%04X,Y", 3},
      [0xe3] = {"ISB ($%02X,X)", 2},
      [0xf3] = {"ISB ($%02X),Y", 2},
      [0x02] = {"HLT", 1},
      [0x12] = {"HLT", 1},
      [0x22] = {"HLT", 1},
      [0x32] = {"HLT", 1},
      [0x42] = {"HLT", 1},
      [0x52] = {"HLT", 1},
      [0x62] = {"HLT", 1},
      [0x72] = {"HLT", 1},
      [0x92] = {"HLT", 1},
      [0xb2] = {"HLT", 1},
      [0xd2] = {"HLT", 1},
      [0xf2] = {"HLT", 1},
      [0xbb] = {"LAE $%04X,Y", 3},
      [0xa7] = {"LAX $%02X", 2},
      [0xb7] = {"LAX $%02X,Y", 2},
      [0xaf] = {"LAX $%04X", 3},
      [0xbf] = {"LAX $%04X,Y", 3},
      [0xa3] = {"LAX ($%02X,X)", 2},
      [0xb3] = {"LAX ($%02X),Y", 2},
      [0xab] = {"LXA #$%02X", 2},
      [0x1a] = {"NOP", 1},
      [0x3a] = {"NOP", 1},
      [0x5a] = {"NOP", 1},
      [0x7a] = {"NOP", 1},
      [0xda] = {"NOP", 1},
      [0xfa] = {"NOP", 1},
      [0x80] = {"NOP #$%02X", 2},
      [0x82] = {"NOP #$%02X", 2},
      [0x89] = {"NOP #$%02X", 2},
      [0xc2] = {"NOP #$%02X", 2},
      [0xe2] = {"NOP #$%02X", 2},
      [0x04] = {"NOP $%02X", 2},
      [0x44] = {"NOP $%02X", 2},
      [0x64] = {"NOP $%02X", 2},
      [0x14] = {"NOP $%02X,X", 2},
      [0x34] = {"NOP $%02X,X", 2},
      [0x54] = {"NOP $%02X,X", 2},
      [0x74] = {"NOP $%02X,X", 2},
      [0xd4] = {"NOP $%02X,X", 2},
      [0xf4] = {"NOP $%02X,X", 2},
      [0x0c] = {"NOP $%04X", 3},
      [0x1c] = {"NOP $%04X,X", 3},
      [0x3c] = {"NOP $%04X,X", 3},
      [0x5c] = {"NOP $%04X,X", 3},
      [0x7c] = {"NOP $%04X,X", 3},
      [0xdc] = {"NOP $%04X,X", 3},
      [0xfc] = {"NOP $%04X,X", 3},
      [0x27] = {"RLA $%02X", 2},
      [0x37] = {"RLA $%02X,X", 2},
      [0x2f] = {"RLA $%04X", 3},
      [0x3f] = {"RLA $%04X,X", 3},
      [0x3b] = {"RLA $%04X,Y", 3},
      [0x23] = {"RLA ($%02X,X)", 2},
      [0x33] = {"RLA ($%02X),Y", 2},
      [0x67] = {"RRA $%02X", 2},
      [0x77] = {"RRA $%02X,X", 2},
      [0x6f] = {"RRA $%04X", 3},
      [0x7f] = {"RRA $%04X,X", 3},
      [0x7b] = {"RRA $%04X,Y", 3},
      [0x63] = {"RRA ($%02X,X)", 2},
      [0x73] = {"RRA ($%02X),Y", 2},
      [0x87] = {"SAX $%02X", 2},
      [0x97] = {"SAX $%02X,Y", 2},
      [0x8f] = {"SAX $%04X", 3},
      [0x83] = {"SAX ($%02X,X)", 2},

      //[0xef] = {"SBC $%06X", 4},
      //[0xff] = {"SBC $%06X,X", 4},
      //[0xf2] = {"SBC ($%02X)", 2},
      //[0xe7] = {"SBC [$%02X]", 2},
      //[0xf7] = {"SBC [$%02X],Y", 2},
      //[0xe3] = {"SBC $%02X,S", 2},
      //[0xf3] = {"SBC ($%02X,S),Y", 2},
      [0xcb] = {"SBX $%02X", 2},
      [0x93] = {"SHA $%04X,X", 3},
      [0x9f] = {"SHA $%04X,Y", 3},
      [0x9b] = {"SHS $%04X,Y", 3},
      [0x9e] = {"SHX $%04X,Y", 3},
      [0x9c] = {"SHY $%04X,X", 3},
      [0x07] = {"SLO $%02X", 2},
      [0x17] = {"SLO $%02X,X", 2},
      [0x0f] = {"SLO $%04X", 3},
      [0x1f] = {"SLO $%04X,X", 3},
      [0x1b] = {"SLO $%04X,Y", 3},
      [0x03] = {"SLO ($%02X,X)", 2},
      [0x13] = {"SLO ($%02X),Y", 2},
      [0x47] = {"SRE $%02X", 2},
      [0x57] = {"SRE $%02X,X", 2},
      [0x4f] = {"SRE $%04X", 3},
      [0x5f] = {"SRE $%04X,X", 3},
      [0x5b] = {"SRE $%04X,Y", 3},
      [0x43] = {"SRE ($%02X,X)", 2},
      [0x53] = {"SRE ($%02X),Y", 2},
#endif /* UNOFFICIAL_OPCODES */
  };
  char *format_buffer = calloc(1024, sizeof(char));
  if (format_buffer == NULL) {
    fprintf(stderr, "Could not allocate format_buffer\n");
    return;
  }
  char buffer[516];
  unsigned char opcode = read_byte_at(&processor, processor.registers.pc++);
  opcode_t info = ops[opcode];
  uint16_t arg = 0;
  const char *byte_strings[] = {"%.2X        ", "%.2X %.2X     ",
                                "%.2X %.2X %.2X  "};

  if (info.format == NULL) {
    fprintf(fp, "error: opcode $%x\n", opcode);
    printf("Error invalid opcode: $%x, pc: $%x\n", opcode,
           processor.registers.pc - 1);
    exit(1);
  }

  if (info.len == 3) {
    arg = read_word_at(&processor, processor.registers.pc);
  } else if (info.len == 2) {
    arg = read_byte_at(&processor, processor.registers.pc);
  }

  sprintf(buffer, "%.4X\t", processor.registers.pc - 1);
  strncat(format_buffer, buffer, 1023);
  sprintf(buffer, byte_strings[info.len - 1], opcode, arg & 0xff, arg >> 8);
  strcat(format_buffer, buffer);
  sprintf(buffer, info.format, arg);
  strcat(format_buffer, buffer);

  if (info.len == 1) {
    strcat(format_buffer, "\t");
  }
  sprintf(buffer,
          "\t\tA: %02X X: %02X Y: %02X SP: %02X P: %.02X CYC: %llu\n",
          processor.registers.accumulator, processor.registers.x,
          processor.registers.y, processor.registers._sp,
          processor.registers.status, processor.clock_ticks);

  strcat(format_buffer, buffer);
  if (fp != NULL) {
    fprintf(fp, "%s", format_buffer);
  }
#ifdef DEBUG
  printf(format_buffer);
#endif
  free(format_buffer);
}

void close_log() {
  puts("freeing log");
  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }
}
