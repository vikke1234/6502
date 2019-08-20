#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/logger.h"

/** @deprecated */
typedef enum {
  IMPLICT,
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

static FILE *fp;

static addressing_modes_t decode_addressing_mode(uint8_t opcode) {
  addressing_modes_t modes10[] = {[0x00] = IMMEDIATE,   [0x01] = ZERO_PAGE,
                                  [0x02] = ACCUMULATOR, [0x03] = ABSOLUTE,
                                  [0x05] = ZERO_PAGE_X, [0x07] = ABSOLUTE_X};

  addressing_modes_t modes01[] = {[0x00] = INDIRECT_X, [0x01] = ZERO_PAGE,
                                  [0x02] = IMMEDIATE,  [0x03] = ABSOLUTE,
                                  [0x04] = INDIRECT_Y, [0x05] = ZERO_PAGE_X,
                                  [0x06] = ABSOLUTE_Y, [0x07] = ABSOLUTE_X};

  addressing_modes_t modes00[] = {[0x00] = IMMEDIATE,
                                  [0x01] = ZERO_PAGE,
                                  [0x03] = ABSOLUTE,
                                  [0x05] = ZERO_PAGE_X,
                                  [0x07] = ABSOLUTE_X};
  const uint8_t type_mask = 0x3;
  const uint8_t mode_mask = 0x1c;
  /* this is done to check which type of opcode
   * we're dealing with as there's 4 different ways to decode it*/
  const uint8_t type = opcode & type_mask;
  const uint8_t mode = (opcode & mode_mask) >> 2;

  /* TODO make rules for when you're trying to get access to an addressing mode
   * that doesn't exist for the given instruction for now it assumes everything
   * has a respective addressing mode */
  switch (type) {
  case 0x0:
    return modes00[mode];
  case 0x1:
    return modes01[mode];
  case 0x2:
    return modes10[mode];
  default:
    return UNKNOWN;
  }
}

void init_log(void) {
  time_t now;
  struct tm *t;
  time(&now);
  t = localtime(&now);
  char buffer[256];
  strftime(buffer, sizeof(buffer), "%c", t);
  strcat(buffer, ".log");
  printf("opening: %s\n", buffer);

  fp = fopen(buffer, "w");
}

/**
   @brief writes to a log file, if debug build it also prints to stdio
   this will probably be expanded on, to add the instructions
   @param reg cpu registers
 */
void log_cpu(registers_t reg) {
  char buffer[256];
  char *format[] = {
      [ACCUMULATOR] = "%s %x",       [IMMEDIATE] = "%s #$%x",
      [ZERO_PAGE] = "%s %x",         [ABSOLUTE] = "%s %x",
      [ZERO_PAGE_Y] = "%s $%x, $%x", [ZERO_PAGE_X] = "%s $%x, $%x",
      [ABSOLUTE_X] = "%s $%x, $%x",  [ABSOLUTE_Y] = "%s $%x, $%x",
      [INDIRECT_X] = "%s ($%x, %x)", [INDIRECT_Y] = "%s ($%x), %x"};

  sprintf(buffer, "PC: %.4x A: 0x%x X: 0x%x Y: 0x%x P: 0x%x\n", reg.pc,
          reg.accumulator, reg.x, reg.y, reg.status);

  fprintf(fp,"%s", buffer);
#ifdef DEBUG
  puts(buffer);
#endif
}

void close_log() {
  if (fp != NULL) {
    fclose(fp);
  }
}
