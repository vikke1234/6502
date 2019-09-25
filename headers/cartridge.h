#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>
typedef struct {
  _Bool nes2;
  union {
    uint8_t header[16];
    struct {
      uint8_t constant[4];
      uint8_t prg_rom_size;
      uint8_t chr_rom_size;
      uint8_t flags6;
      uint8_t flags7;
      /* other flags not supported */
    };
  } header;
  uint8_t *prg_rom;
  uint8_t *chr_rom;
} cartridge_t;

extern cartridge_t *open_program(const char * const path);
extern void free_cartridge(cartridge_t *cart);
#endif /* CARTRIDGE_H */
