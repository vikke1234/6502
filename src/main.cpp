#include <iostream>
#include <stdio.h>
#include "../headers/file_handler.h"
#include "../headers/cpu.h"
#include <sys/stat.h>
#include <map>
void print_help();

int main(int argc, char *argv[])
{
	//FILE *file = fopen("/home/viho/Downloads/SMB.nes", "rb");
  uint8_t data[4];
	data[0] = 0x69;
	data[1] = 0x2d;
	data[2] = 0x0e;

	for (int i = 0; i < 3; i++) {
		interpret_opcode(data[i]);
	}

	return 0;
}

void print_help()
{
	std::cout << "./emulator game.nes\n";
}
