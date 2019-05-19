#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
  IMPLICT,
  ACCUMULATOR,
  IMMEDIATE,
  ZERO_PAGE,
  ZERO_PAGE_X,
  ZERO_PAGE_Y,
  RELATIVE,
  IMPLIED,
  ABSOLUTE,
  ABSOLUTE_X,
  ABSOLUTE_Y,
  INDIRECT,
  INDIRECT_X,
  INDIRECT_Y,
  UNKNOWN
} addressing_modes_t;

/* this is for editing processor status */
typedef enum {
  NEGATIVE    = 0x1 << 7,
  OVERFLOW    = 0x1 << 6,
  /* flag in 0x1 << 5 is always one */
  BREAK       = 0x1 << 4,
  DECIMAL     = 0x1 << 3,
  INTERRUPT   = 0x1 << 2,
  ZERO        = 0x1 << 1,
  CARRY       = 0x1 << 0
} flags_t;


/* of type unsigned char instead of uint8_t because uint8_t doesn't allow aliasing */
typedef struct {
  unsigned char RAM              [0x1fff]; /* RAM size 0x800, mirrored 3 times */
  unsigned char ppu_registers    [0x2000]; /* actual size 0x8, repeats every 8 bytes */
  unsigned char apu_registers    [0x18];
  unsigned char test_registers   [0x8];    /* for when the CPU is in test mode */
  unsigned char rom              [0xbfe0]; /* ROM space and mapper registers */
} memory_map;

typedef struct {
  uint16_t pc;
  uint8_t stack_pointer[256];
  uint8_t _sp; /* index where to place things in the stack */
  uint8_t x, y, accumulator; /* x, y and accumulator registers */
  uint8_t status;          /** NVsB DIZC, @see FLAGS */
} processor_registers;

/* maybe use this for nice bundling dunno? would reduce globals which is nice (clock_ticks)*/
typedef struct {
  processor_registers registers;
  memory_map memory;
  unsigned long long clock_ticks;
} processor_t;

extern void interpret_opcode(void);
extern void initialize_cpu(const unsigned char *data, size_t size, memory_map *m, processor_registers *reg);
extern unsigned long long clock_ticks; /* this is for the PPU to be able to time the draws */

#endif // CPU_H
