#include "../headers/cpu.h"
#include "../headers/file_handler.h"
#include <iostream>
#include <map>
#include <stdio.h>
#include <sys/stat.h>
void print_help();

int main(int argc, char *argv[]) {
  // FILE *file = fopen("/home/viho/Downloads/SMB.nes", "rb");
  uint8_t data[4];
  data[0] = 0xa9;
  data[1] = 0x2d;
  data[3] = 0x6a;

  initialize_cpu(data, sizeof(data));
  for (int i = 0; i < 4; i++) {
    interpret_opcode();
  }

  return 0;
}

void print_help() { std::cout << "./emulator game.nes\n"; }
