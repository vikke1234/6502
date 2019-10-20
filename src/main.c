#include "../headers/cpu.h"
#include <stdio.h>

void print_help();

int main(int argc, char *argv[]) {
  if (argc < 2) {
    puts("Please specify file name");
    exit(1);
  }

  if (initialize_cpu_filename(argv[1]) == false) {
    fprintf(stderr,
            "Error: could not initialize cpu, file could no be accessed");
    return 1;
  }

  while(1) {
    interpret_opcode();
  }
  return 0;
}
