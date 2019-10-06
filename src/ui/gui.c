#include "../../headers/cpu.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

void print_registers(WINDOW **windows);
void print_help(void);

#define REGISTER_AMOUNT 5


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
      print_help();
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

void print_help(void) {}

void print_registers(WINDOW **windows) {
  uint8_t unpacked[REGISTER_AMOUNT]; /* see line below */
  static const char *const(strings[REGISTER_AMOUNT]) = {"PC", "Accumulator", "X register",
                                         "Yregister", "SP"};

  registers_t registers = dump_registers();
  unpacked[0] = registers.pc;
  unpacked[1] = registers.x;
  unpacked[2] = registers.y;
  unpacked[3] = registers._sp;
  unpacked[4] = registers.status;

  /* BUG */
  for (int i = 0; i < REGISTER_AMOUNT; i++) {
    mvwprintw(windows[i], 1, 1, "%s\n", strings[i]);
    mvwprintw(windows[i], 2, 1, "0x%x", unpacked[i]);
    wrefresh(windows[i]);
  }
}
