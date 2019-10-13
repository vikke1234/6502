#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/cartridge.h"

extern cartridge_t *open_program(const char *const path) {
  if (path == NULL) {
    exit(1);
  }
  FILE *fp = fopen(path, "rb");

  cartridge_t *cartridge = malloc(sizeof(cartridge_t));
  if (cartridge == NULL) {
    return NULL;
  }

  fread(&cartridge->header, 16, sizeof(unsigned char), fp);

  static unsigned char const type[] = {0x4e, 0x45, 0x53, 0x1a};
  static const int32_t nes_constant = 0x4e45531a;
  if (cartridge->header.constant == nes_constant) {
    fprintf(stderr, "Error: not a NES file");
    exit(1);
  }

  printf("%x, prg size: %x, chr size: %d, flags 6: %x, flags7: %x\n",
         cartridge->header.constant, cartridge->header.prg_rom_size,
         cartridge->header.chr_rom_size, cartridge->header.flags6,
         cartridge->header.flags7);

  cartridge->nes2 = cartridge->header.header[7] & 0x0c;

  cartridge->prg_rom = calloc((size_t)0x4000 * cartridge->header.prg_rom_size,
                              1); /* 0x4000 magic number for size of prg_rom */
  cartridge->chr_rom = calloc((size_t) 0x2000 * cartridge->header.chr_rom_size,
                              1); /* 0x2000 magic number for size of chr_rom */
  if (cartridge->prg_rom == NULL) {
    free(cartridge);
    return NULL;
  }

  if (cartridge->chr_rom == NULL) {
    free(cartridge->prg_rom);
    free(cartridge);
    return NULL;
  }

  int mapper = ((cartridge->header.flags6 & 0xf0) >> 4) |
               (cartridge->header.flags7 & 0xf0);

  if (cartridge->header.flags6 & 0x8) {
    fread(cartridge->prg_rom, 1, 512, fp);
  }

  fread(cartridge->prg_rom, 1, (size_t)0x4000 * cartridge->header.prg_rom_size, fp);
  fread(cartridge->chr_rom, 1, (size_t)0x2000 * cartridge->header.chr_rom_size,
        fp);

  fclose(fp);

  return cartridge;
}

extern void free_cartridge(cartridge_t *c) {
  if (c != NULL) {
    if (c->prg_rom != NULL) {
      free(c->prg_rom);
    }
    if (c->chr_rom != NULL) {
      free(c->chr_rom);
    }
    free(c);
  }
}
