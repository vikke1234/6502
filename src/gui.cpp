#include "../headers/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

void print_registers(void);
void print_help(void);

int main(int argc, char *argv[]) {
  if (argc < 3) {
    puts("Please specify file name");
    exit(1);
  }
 
  initialize_cpu_filename(argv[1]);

  char input = '\0';
  while (input != 'e') {
    scanf("%c", &input);
    switch (input) {
    case 'H':
    case 'h':
      print_help();
      break;
    case 'N':
    case 'n':
      interpret_opcode();
      print_registers();
    case 'P':
    case 'p':
      print_registers();
      break;

    default:
      print_help();
      break;
    }
  }
  return 0;
}

void print_help(void) {
  puts("H/h: show this message");
  puts("{enter}/n/N: next instruction");
  puts("P/p: dump registers");
}
void print_registers(void) {

  fflush(stdout);

  puts("______________________________________________");
  puts("| PC    | Accumulator | X register   | Y register |");
  printf("| 0x%.3x | %.3u         | %.3u          | %.3u        |\n", 0, 0, 0,
         0);
  puts("----------------------------------------------");
}