#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/logger.h"

FILE *fp;

void init_log(void) {
  time_t now;
  struct tm *t = localtime(&now);
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
  sprintf(buffer, "PC: %.4x A: 0x%x X: 0x%x Y: 0x%x P: 0x%x\n", reg.pc, reg.accumulator, reg.x, reg.y, reg.status);
  #ifdef DEBUG
  puts(buffer);
  #endif
}

void close_log() {
  fclose(fp);
}
