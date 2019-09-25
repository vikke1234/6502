#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/logger.h"

/** @deprecated */
typedef enum {
  ACCUMULATOR,
  IMMEDIATE,
  ZERO_PAGE,
  ZERO_PAGE_X,
  ZERO_PAGE_Y,
  RELATIVE,
  IMPLIED,
  ABSOLUTE,
  ABSOLUTE_X,
  ABSOLUTE_Y,
  INDIRECT,
  INDIRECT_X,
  INDIRECT_Y,
  UNKNOWN
} addressing_modes_t;

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
}

char *get_str(addressing_modes_t a) {
  static char *modes[] = {
      [IMMEDIATE] = "immediate",     [ACCUMULATOR] = "accumulator",
      [ZERO_PAGE] = "zero page",     [ABSOLUTE] = "absolute",
      [ZERO_PAGE_Y] = "zero page y", [ZERO_PAGE_X] = "zero page x",
      [ABSOLUTE_X] = "absolute x",   [ABSOLUTE_Y] = "absolute y",
      [INDIRECT_X] = "indirect x",   [INDIRECT_Y] = "indirect y",
      [IMPLIED] = "implied",         [RELATIVE] = "relative",
      [UNKNOWN] = "unknown"};
  return modes[a];
}
/**
   @brief writes to a log file, if debug build it also prints to stdio
   this will probably be expanded on, to add the instructions

   this is a terribly written function which will probably never be optimized
   @param reg cpu registers
 */
void log_cpu(processor_t processor) {
  char buffer[256];
  typedef struct {
    char *format;
    uint8_t len;
  } opcode_t;

  /* formats from radare source
   * https://github.com/radare/radare2/blob/master/libr/asm/arch/6502/6502dis.c
   */
  static opcode_t const ops[] = {
      [0x01] = {"ora (0x%02x, x)", 2},
      [0x05] = {"ora 0x%02x", 2},
      [0x06] = {"asl 0x%02x", 2},
      [0x08] = {"php ", 1},
      [0x09] = {"ora #0x%02x", 2},
      [0x0a] = {"asl A", 1},
      [0x0d] = {"ora 0x%04x", 3},
      [0x0e] = {"asl 0x%04x", 3},
      [0x10] = {"bpl 0x%02x", 2},
      [0x11] = {"ora (0x%02x), y", 2},
      [0x15] = {"ora 0x%02x, x", 2},
      [0x16] = {"asl 0x%02x, x", 2},
      [0x18] = {"clc ", 1},
      [0x19] = {"ora 0x%04x, y", 3},
      [0x1d] = {"ora 0x%04x, x", 3},
      [0x1e] = {"asl 0x%04x, x", 3},
      [0x20] = {"jsr 0x%04x", 3},
      [0x21] = {"and (0x%02x, x)", 2},
      [0x24] = {"bit 0x%02x", 2},
      [0x25] = {"and 0x%02x", 2},
      [0x26] = {"rol 0x%02x", 2},
      [0x28] = {"plp ", 1},
      [0x29] = {"and #0x%02x", 2},
      [0x2a] = {"rol A", 1},
      [0x2c] = {"bit 0x%04x", 3},
      [0x2d] = {"and 0x%04x", 3},
      [0x2e] = {"rol 0x%04x", 3},
      [0x30] = {"bmi 0x%02x", 2},
      [0x31] = {"and (0x%02x), y", 2},
      [0x35] = {"and 0x%02x, x", 2},
      [0x36] = {"rol 0x%02x, x", 2},
      [0x38] = {"sec ", 1},
      [0x39] = {"and 0x%04x, y", 3},
      [0x3d] = {"and 0x%04x, x", 3},
      [0x3e] = {"rol 0x%04x, x", 3},
      [0x40] = {"rti ", 1},
      [0x41] = {"eor (0x%02x, x)", 2},
      [0x45] = {"eor 0x%02x", 2},
      [0x46] = {"lsr 0x%02x", 2},
      [0x48] = {"pha ", 1},
      [0x49] = {"eor #0x%02x", 2},
      [0x4a] = {"lsr A", 1},
      [0x4c] = {"jmp 0x%04x", 3},
      [0x4d] = {"eor 0x%04x", 3},
      [0x4e] = {"lsr 0x%04x", 3},
      [0x50] = {"bvc 0x%02x", 2},
      [0x51] = {"eor (0x%02x), y", 2},
      [0x55] = {"eor 0x%02x, x", 2},
      [0x56] = {"lsr 0x%02x, x", 2},
      [0x58] = {"cli ", 1},
      [0x59] = {"eor 0x%04x, y", 3},
      [0x5d] = {"eor 0x%04x, x", 3},
      [0x5e] = {"lsr 0x%04x, x", 3},
      [0x60] = {"rts ", 1},
      [0x61] = {"adc (0x%02x, x)", 2},
      [0x65] = {"adc 0x%02x", 2},
      [0x66] = {"ror 0x%02x", 2},
      [0x68] = {"pla ", 1},
      [0x69] = {"adc #0x%02x", 2},
      [0x6a] = {"ror A", 1},
      [0x6c] = {"jmp (0x%04x)", 3},
      [0x6d] = {"adc 0x%04x", 3},
      [0x6e] = {"ror 0x%04x", 3},
      [0x70] = {"bvs 0x%02x", 2},
      [0x71] = {"adc (0x%02x), y", 2},
      [0x75] = {"adc 0x%02x, x", 2},
      [0x76] = {"ror 0x%02x, x", 2},
      [0x78] = {"sei ", 1},
      [0x79] = {"adc 0x%04x, y", 3},
      [0x7d] = {"adc 0x%04x, x", 3},
      [0x7e] = {"ror 0x%04x, x", 3},
      [0x81] = {"sta (0x%02x, x)", 2},
      [0x84] = {"sty 0x%02x", 2},
      [0x85] = {"sta 0x%02x", 2},
      [0x86] = {"stx 0x%02x", 2},
      [0x88] = {"dey ", 1},
      [0x8a] = {"txa ", 1},
      [0x8c] = {"sty 0x%04x", 3},
      [0x8d] = {"sta 0x%04x", 3},
      [0x8e] = {"stx 0x%04x", 3},
      [0x90] = {"bcc 0x%02x", 2},
      [0x91] = {"sta (0x%02x), y", 2},
      [0x94] = {"sty 0x%02x, x", 2},
      [0x95] = {"sta 0x%02x, x", 2},
      [0x96] = {"stx 0x%02x, y", 2},
      [0x98] = {"tya ", 1},
      [0x99] = {"sta 0x%04x, y", 3},
      [0x9a] = {"txs ", 1},
      [0x9d] = {"sta 0x%04x, x", 3},
      [0xa0] = {"ldy #0x%02x", 2},
      [0xa1] = {"lda (0x%02x, x)", 2},
      [0xa2] = {"ldx #0x%02x", 2},
      [0xa4] = {"ldy 0x%02x", 2},
      [0xa5] = {"lda 0x%02x", 2},
      [0xa6] = {"ldx 0x%02x", 2},
      [0xa8] = {"tay ", 1},
      [0xa9] = {"lda #0x%02x", 2},
      [0xaa] = {"tax ", 1},
      [0xac] = {"ldy 0x%04x", 3},
      [0xad] = {"lda 0x%04x", 3},
      [0xae] = {"ldx 0x%04x", 3},
      [0xb0] = {"bcs 0x%02x", 2},
      [0xb1] = {"lda (0x%02x), y", 2},
      [0xb4] = {"ldy 0x%02x, x", 2},
      [0xb5] = {"lda 0x%02x, x", 2},
      [0xb6] = {"ldx 0x%02x, y", 2},
      [0xb8] = {"clv ", 1},
      [0xb9] = {"lda 0x%04x, y", 3},
      [0xba] = {"tsx ", 1},
      [0xbc] = {"ldy 0x%04x, x", 3},
      [0xbd] = {"lda 0x%04x, x", 3},
      [0xbe] = {"ldx 0x%04x, y", 3},
      [0xc0] = {"cpy #0x%02x", 2},
      [0xc1] = {"cmp (0x%02x, x)", 2},
      [0xc4] = {"cpy 0x%02x", 2},
      [0xc5] = {"cmp 0x%02x", 2},
      [0xc6] = {"dec 0x%02x", 2},
      [0xc8] = {"iny ", 1},
      [0xc9] = {"cmp #0x%02x", 2},
      [0xca] = {"dex ", 1},
      [0xcc] = {"cpy 0x%04x", 3},
      [0xcd] = {"cmp 0x%04x", 3},
      [0xce] = {"dec 0x%04x", 3},
      [0xd0] = {"bne 0x%02x", 2},
      [0xd1] = {"cmp (0x%02x), y", 2},
      [0xd5] = {"cmp 0x%02x, x", 2},
      [0xd6] = {"dec 0x%02x, x", 2},
      [0xd8] = {"cld ", 1},
      [0xd9] = {"cmp 0x%04x, y", 3},
      [0xdd] = {"cmp 0x%04x, x", 3},
      [0xde] = {"dec 0x%04x, x", 3},
      [0xe0] = {"cpx #0x%02x", 2},
      [0xe4] = {"cpx 0x%02x", 2},
      [0xe6] = {"inc 0x%02x", 2},
      [0xe8] = {"inx ", 1},
      [0xec] = {"cpx 0x%04x", 3},
      [0xee] = {"inc 0x%04x", 3},
      [0xf0] = {"beq 0x%02x", 2},
      [0xf6] = {"inc 0x%02x, x", 2},
      [0xf8] = {"sed ", 1},
      [0xfe] = {"inc 0x%04x, x", 3},

      [0x00] = {"brk", 1},
      [0x0b] = {"anc #0x%02x", 2},
      [0x2b] = {"anc #0x%02x", 2},
      [0x8b] = {"ane #0x%02x", 2},
      [0x6b] = {"arr #0x%02x", 2},
      [0x4b] = {"asr #0x%02x", 2},
      [0xc7] = {"dcp 0x%02x", 2},
      [0xd7] = {"dcp 0x%02x,x", 2},
      [0xcf] = {"dcp 0x%04x", 3},
      [0xdf] = {"dcp 0x%04x,x", 3},
      [0xdb] = {"dcp 0x%04x,y", 3},
      [0xc3] = {"dcp (0x%02x,x)", 2},
      [0xd3] = {"dcp (0x%02x),y", 2},
      [0xe7] = {"isb 0x%02x", 2},
      [0xf7] = {"isb 0x%02x,x", 2},
      [0xef] = {"isb 0x%04x", 3},
      [0xff] = {"isb 0x%04x,x", 3},
      [0xfb] = {"isb 0x%04x,y", 3},
      [0xe3] = {"isb (0x%02x,x)", 2},
      [0xf3] = {"isb (0x%02x),y", 2},
      [0x02] = {"hlt", 1},
      [0x12] = {"hlt", 1},
      [0x22] = {"hlt", 1},
      [0x32] = {"hlt", 1},
      [0x42] = {"hlt", 1},
      [0x52] = {"hlt", 1},
      [0x62] = {"hlt", 1},
      [0x72] = {"hlt", 1},
      [0x92] = {"hlt", 1},
      [0xb2] = {"hlt", 1},
      [0xd2] = {"hlt", 1},
      [0xf2] = {"hlt", 1},
      [0xbb] = {"lae 0x%04x,y", 3},
      [0xa7] = {"lax 0x%02x", 2},
      [0xb7] = {"lax 0x%02x,y", 2},
      [0xaf] = {"lax 0x%04x", 3},
      [0xbf] = {"lax 0x%04x,y", 3},
      [0xa3] = {"lax (0x%02x,x)", 2},
      [0xb3] = {"lax (0x%02x),y", 2},
      [0xab] = {"lxa #0x%02x", 2},
      [0xea] = {"nop", 1},
      [0x1a] = {"nop", 1},
      [0x3a] = {"nop", 1},
      [0x5a] = {"nop", 1},
      [0x7a] = {"nop", 1},
      [0xda] = {"nop", 1},
      [0xfa] = {"nop", 1},
      [0x80] = {"nop #0x%02x", 2},
      [0x82] = {"nop #0x%02x", 2},
      [0x89] = {"nop #0x%02x", 2},
      [0xc2] = {"nop #0x%02x", 2},
      [0xe2] = {"nop #0x%02x", 2},
      [0x04] = {"nop 0x%02x", 2},
      [0x44] = {"nop 0x%02x", 2},
      [0x64] = {"nop 0x%02x", 2},
      [0x14] = {"nop 0x%02x,x", 2},
      [0x34] = {"nop 0x%02x,x", 2},
      [0x54] = {"nop 0x%02x,x", 2},
      [0x74] = {"nop 0x%02x,x", 2},
      [0xd4] = {"nop 0x%02x,x", 2},
      [0xf4] = {"nop 0x%02x,x", 2},
      [0x0c] = {"nop 0x%04x", 3},
      [0x1c] = {"nop 0x%04x,x", 3},
      [0x3c] = {"nop 0x%04x,x", 3},
      [0x5c] = {"nop 0x%04x,x", 3},
      [0x7c] = {"nop 0x%04x,x", 3},
      [0xdc] = {"nop 0x%04x,x", 3},
      [0xfc] = {"nop 0x%04x,x", 3},
      [0x27] = {"rla 0x%02x", 2},
      [0x37] = {"rla 0x%02x,x", 2},
      [0x2f] = {"rla 0x%04x", 3},
      [0x3f] = {"rla 0x%04x,x", 3},
      [0x3b] = {"rla 0x%04x,y", 3},
      [0x23] = {"rla (0x%02x,x)", 2},
      [0x33] = {"rla (0x%02x),y", 2},
      [0x67] = {"rra 0x%02x", 2},
      [0x77] = {"rra 0x%02x,x", 2},
      [0x6f] = {"rra 0x%04x", 3},
      [0x7f] = {"rra 0x%04x,x", 3},
      [0x7b] = {"rra 0x%04x,y", 3},
      [0x63] = {"rra (0x%02x,x)", 2},
      [0x73] = {"rra (0x%02x),y", 2},
      [0x87] = {"sax 0x%02x", 2},
      [0x97] = {"sax 0x%02x,y", 2},
      [0x8f] = {"sax 0x%04x", 3},
      [0x83] = {"sax (0x%02x,x)", 2},
      [0xe9] = {"sbc #0x%02x", 2},
      [0xe5] = {"sbc 0x%02x", 2},
      [0xf5] = {"sbc 0x%02x,x", 2},
      [0xed] = {"sbc 0x%04x", 3},
      [0xfd] = {"sbc 0x%04x,x", 3},
      [0xf9] = {"sbc 0x%04x,y", 3},
      [0xe1] = {"sbc (0x%02x,x)", 2},
      [0xf1] = {"sbc (0x%02x),y", 2},
      [0xeb] = {"sbc #0x%02x", 2},
      //[0xef] = {"sbc 0x%06x", 4},
      //[0xff] = {"sbc 0x%06x,x", 4},
      //[0xf2] = {"sbc (0x%02x)", 2},
      //[0xe7] = {"sbc [0x%02x]", 2},
      //[0xf7] = {"sbc [0x%02x],y", 2},
      //[0xe3] = {"sbc 0x%02x,s", 2},
      //[0xf3] = {"sbc (0x%02x,s),y", 2},
      [0xcb] = {"sbx 0x%02x", 2},
      [0x93] = {"sha 0x%04x,x", 3},
      [0x9f] = {"sha 0x%04x,y", 3},
      [0x9b] = {"shs 0x%04x,y", 3},
      [0x9e] = {"shx 0x%04x,y", 3},
      [0x9c] = {"shy 0x%04x,x", 3},
      [0x07] = {"slo 0x%02x", 2},
      [0x17] = {"slo 0x%02x,x", 2},
      [0x0f] = {"slo 0x%04x", 3},
      [0x1f] = {"slo 0x%04x,x", 3},
      [0x1b] = {"slo 0x%04x,y", 3},
      [0x03] = {"slo (0x%02x,x)", 2},
      [0x13] = {"slo (0x%02x),y", 2},
      [0x47] = {"sre 0x%02x", 2},
      [0x57] = {"sre 0x%02x,x", 2},
      [0x4f] = {"sre 0x%04x", 3},
      [0x5f] = {"sre 0x%04x,x", 3},
      [0x5b] = {"sre 0x%04x,y", 3},
      [0x43] = {"sre (0x%02x,x)", 2},
      [0x53] = {"sre (0x%02x),y", 2},

  };
  char format_buffer[1024];
  unsigned char opcode = processor.memory[processor.registers.pc];
  opcode_t info = ops[opcode];
  uint16_t arg = 0;

  if(info.len == 3) {
    arg |= (processor.memory[processor.registers.pc + 2] << 8);
    /* break intentionally left out */
  }
  arg |= processor.memory[processor.registers.pc + 1];

  if (info.format == NULL) {
    printf("error: opcode $%x\n", opcode);
    exit(1);
  }
  sprintf(format_buffer, info.format, arg);
  if(strlen(format_buffer) < 8) {
    strcat(format_buffer, "\t");
  }
  sprintf(
      buffer,
      "\t\tPC: %.4x A: 0x%04x X: 0x%02x Y: 0x%02x SP: 0x%02x P:  c: %d, z: %d, "
      "i: %d, d: "
      "%d, b: %d, V: %d, n: %d\n",
      processor.registers.pc, processor.registers.accumulator,
      processor.registers.x, processor.registers.y, processor.registers._sp,
      processor.registers._status.c, processor.registers._status.z,
      processor.registers._status.i, processor.registers._status.d,
      processor.registers._status.b, processor.registers._status.v,
      processor.registers._status.n);

  strcat(format_buffer, buffer);
  fprintf(fp, "%s", format_buffer);
#ifdef DEBUG
  printf(format_buffer);
#endif
}

void close_log() {
  if (fp != NULL) {
    fclose(fp);
  }
}
