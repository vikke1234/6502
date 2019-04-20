#include "../headers/cpu.h"
#include "../headers/file_handler.h"
#include <iostream>
#include <map>
#include <stdio.h>
#include <sys/stat.h>
void print_help();

int main(int argc, char *argv[]) {
  // FILE *file = fopen("/home/viho/Downloads/SMB.nes", "rb");
  uint8_t data[] = {0xa2, 0x01,
                    0xa9, 0x05,
                    0x85, 0x01,
                    0xa9, 0x07,
                    0x85, 0x02,
                    0xa0, 0x0a,
                    0x8c, 0x05, 0x07,
                    0xa1, 0x00};

  initialize_cpu(data, sizeof(data));
  for (unsigned long i = 0; i < sizeof(data); i++) {
    interpret_opcode();
  }

  return 0;
}

void print_help() { std::cout << "./emulator game.nes\n"; }
