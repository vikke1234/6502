#ifndef CPU_H
#define CPU_H
#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include <stdint.h>
#include <stdlib.h>

#include "cartridge.h"


/* this is for editing processor status */
typedef enum _6502_flags {
  NEGATIVE = 0x1 << 7,
  OVERFLOW = 0x1 << 6,
  BREAK = 0x1 << 4,
  DECIMAL = 0x1 << 3,
  INTERRUPT = 0x1 << 2,
  ZERO = 0x1 << 1,
  CARRY = 0x1 << 0
} flags_t;

enum m6502constants {
  STACK_SIZE = 256,
  STACK_START = 0x0100,
  RAM_SIZE = 0x1fff,
  PPU_REGISTERS_SIZE = 0x2000,
  APU_REGISTERS_SIZE = 0x18,
  TEST_REGISTERS_SIZE = 0x8,
  ROM_MEMORY_SIZE = 0xbfe0,
  TOTAL_MEMORY_SIZE = RAM_SIZE + PPU_REGISTERS_SIZE + APU_REGISTERS_SIZE +
                      TEST_REGISTERS_SIZE + ROM_MEMORY_SIZE
};

enum error_codes6502 { SUCCESS, STACK_OVERFLOW, STACK_UNDERFLOW };

/* of type unsigned char instead of uint8_t because uint8_t doesn't allow
 * aliasing
 * CURRENTLY NOT USED */
typedef struct {
  unsigned char RAM[RAM_SIZE]; /* RAM size 0x800, mirrored 3 times */
  unsigned char ppu_registers[PPU_REGISTERS_SIZE]; /* actual size 0x8, repeats
                                                    every 8 bytes */
  unsigned char apu_registers[APU_REGISTERS_SIZE];
  unsigned char test_registers[TEST_REGISTERS_SIZE]; /* for when the CPU is in
                                                        test mode */
  unsigned char rom[ROM_MEMORY_SIZE]; /* ROM space and mapper registers */
} memory_map;

typedef struct _processor_registers {
  uint16_t pc;
  uint8_t stack_pointer[STACK_SIZE];
  uint8_t _sp;               /* index where to place things in the stack */
  uint8_t x, y, accumulator; /* x, y and accumulator registers */

  union {
    uint8_t status; /** NVsB DIZC, @see FLAGS */
    struct {
      uint8_t n : 1;
      uint8_t v : 1;
      uint8_t : 2;
      uint8_t b : 1;
      uint8_t d : 1;
      uint8_t i : 1;
      uint8_t z : 1;
      uint8_t c : 1;
    } _status;
  };
} registers_t;

/* maybe use this for nice bundling dunno? would reduce globals which is nice
 * (clock_ticks) currently not in use*/
typedef struct {
  uint8_t memory[TOTAL_MEMORY_SIZE + 1];
  registers_t registers;
  unsigned long long clock_ticks;
} processor_t;

extern void interpret_opcode(void);

/**
   @brief initializes the cpu, sets all of the memory addresses to 0xff
   @param cart the cartridge 
*/
extern void initialize_cpu(cartridge_t *cart);
extern bool initialize_cpu_filename(char *path);
extern registers_t dump_registers(void);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CPU_H
