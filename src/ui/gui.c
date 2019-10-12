#include "../../headers/cpu.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

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

  int input = '\0';
  while (input != 'e' && input != 'q') {
    input = 'n';
    switch (input) {
    case 'H':
    case 'h':
      break;
    case 'N':
    case 'n':
    case '\n':
      interpret_opcode();
      break;

    default:
      break;
    }
  }

  return 0;
}
