#include "../headers/cpu.h"
#include "../headers/file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void print_help();

int main(int argc, char *argv[]) {
  if (argc < 2) {
    puts("error no filename given");
    return 0;
  }
  struct stat s;
  stat(argv[1], &s);
  FILE *file = fopen(argv[1], "rb");

  uint8_t *data = (uint8_t *)calloc(sizeof(char), s.st_size);
  fread(data, 1, s.st_size, file);
  initialize_cpu(data, s.st_size);
  for (long i = 0; i < s.st_size - 0x600; i++) {
    interpret_opcode();
  }

  return 0;
}
