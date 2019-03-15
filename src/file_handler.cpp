#include <fstream>
#include <ios>
#include <string.h>
#include "../headers/file_handler.h"
#include <stdlib.h>

struct file *read_rom(char *path) {
    struct file *f = (struct file *) malloc(sizeof (struct file));
    std::ifstream rom;
    rom.open(path, std::ios::binary);
    rom.read((char *) f -> header, 16);
    if(strncmp((const char *) f -> header, "NES", 3) != 0l) {
        return nullptr;
    }
    return f;
}


void free_rom(struct file *f) {
    free(f);
}
