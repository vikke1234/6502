#include "../headers/cpu.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* TODO:
 * add clock counter to make drawing easier
 */

typedef void (*instruction_pointer)(void);

/**
 * @brief sets the flag to the value of b
 * @param flag
 * @param b what the flag should be
 */
static inline void set_flag(flags_t flag, bool b);

/**
 * @brief takes an array of flags and checks for whether they should be set or
 * not
 * @param flags array of flags to be checked
 * @param n amount of flags
 */
static void set_flags(flags_t *flags, int n);

/**
 * @brief gets a given flag, 1 or 0
 * @param flag
 */
static inline uint8_t get_flag(flags_t flag);

/**
 * @brief reads 2 little endian bytes and combines them into
 a 16bit unsigned
 * assumes that thereemacs's atleast 2 bytes to read, note increment PC by 2
 */
static inline uint16_t read_word();

/**
 * @brief reads 1 byte (little endian), note increment PC
 */
static inline uint8_t read_byte();
static inline uint8_t read_byte_at(uint16_t location);
static inline uint16_t read_word_at(uint16_t location);

static processor_registers _registers;
static processor_registers *registers = &_registers;
/* for "easier" access to the different parts of memory for e.g. debugging
 * purpuses */
static memory_map _memory;
static unsigned char *memory = (unsigned char *)&_memory;

/* this will probably be rewritten once the cpu is done */
extern void initialize_cpu(const unsigned char *data, size_t size,
                           memory_map *m, processor_registers *reg) {
  if (m != NULL) {
    memory = (unsigned char *)m;
  }
  if (reg != NULL) {
    registers = reg;
  }
  registers->_sp = 0;
  registers->accumulator = 0;
  registers->pc = 0x600;
  registers->status = 0;
  registers->x = 0;
  registers->y = 0;

  memcpy(&memory[registers->pc], &data[0x600], size - 0x600);
}

extern void interpret_opcode(void) {
  /* figure out how to reduce the amount of shit in this? */
  static instruction_pointer instructions[255] = {[0x69] = &ADC_im,
                                                  [0x65] = &ADC_zero,
                                                  [0x75] = &ADC_zerox,
                                                  [0x6D] = &ADC_absolute,
                                                  [0x7D] = &ADC_absolutex,
                                                  [0x79] = &ADC_absolutey,
                                                  [0x61] = &ADC_indirectx,
                                                  [0x71] = &ADC_indirecty,

                                                  [0x29] = &AND_im,
                                                  [0x25] = &AND_zero,
                                                  [0x35] = &AND_zerox,
                                                  [0x2D] = &AND_absolute,
                                                  [0x3D] = &AND_absolutex,
                                                  [0x39] = &AND_absolutey,
                                                  [0x21] = &AND_indirectx,
                                                  [0x31] = &AND_indirecty,

                                                  [0x0A] = &ASL_accumulator,
                                                  [0x06] = &ASL_zero,
                                                  [0x16] = &ASL_zerox,
                                                  [0x0E] = &ASL_absolute,
                                                  [0x1E] = &ASL_absolutex,

                                                  [0x90] = &BCC,
                                                  [0xB0] = &BCS,
                                                  [0xF0] = &BEQ,

                                                  [0x24] = &BIT_zero,
                                                  [0x2C] = &BIT_absolute,

                                                  [0x30] = &BMI,
                                                  [0xD0] = &BNE,
                                                  [0x10] = &BPL,
                                                  [0x00] = &BRK,
                                                  [0x50] = &BVC,
                                                  [0x70] = &BVS,

                                                  [0x18] = &CLC,
                                                  [0xD8] = &CLD,
                                                  [0x58] = &CLI,
                                                  [0xB8] = &CLV,

                                                  [0xC9] = &CMP_im,
                                                  [0xC5] = &CMP_zero,
                                                  [0xD5] = &CMP_zerox,
                                                  [0xCD] = &CMP_absolute,
                                                  [0xDD] = &CMP_absolutex,
                                                  [0xD9] = &CMP_absolutey,
                                                  [0xC1] = &CMP_indirectx,
                                                  [0xD1] = &CMP_indirecty,

                                                  [0xE0] = &CPX_im,
                                                  [0xE4] = &CPX_zero,
                                                  [0xEC] = &CPX_absolute,
                                                  [0xC0] = &CPY_im,
                                                  [0xC4] = &CPY_zero,
                                                  [0xCC] = &CPY_absolute,

                                                  [0xC6] = &DEC_zero,
                                                  [0xD6] = &DEC_zerox,
                                                  [0xCE] = &DEC_absolute,
                                                  [0xDE] = &DEC_absolutex,
                                                  [0xCA] = &DEX,
                                                  [0x88] = &DEY,

                                                  [0x49] = &EOR_im,
                                                  [0x45] = &EOR_zero,
                                                  [0x55] = &EOR_zerox,
                                                  [0x4D] = &EOR_absolute,
                                                  [0x5D] = &EOR_absolutex,
                                                  [0x59] = &EOR_absolutey,
                                                  [0x41] = &EOR_indirectx,
                                                  [0x51] = &EOR_indirecty,

                                                  [0xE6] = &INC_zero,
                                                  [0xF6] = &INC_zerox,
                                                  [0xEE] = &INC_absolute,
                                                  [0xFE] = &INC_absolutex,
                                                  [0xE8] = &INX,
                                                  [0xC8] = &INY,
                                                  [0x4C] = &JMP_absolute,
                                                  [0x6C] = &JMP_indirect,
                                                  [0x20] = &JSR,

                                                  [0xA9] = &LDA_im,
                                                  [0xA5] = &LDA_zero,
                                                  [0xB5] = &LDA_zerox,
                                                  [0xAD] = &LDA_absolute,
                                                  [0xBD] = &LDA_absolutex,
                                                  [0xB9] = &LDA_absolutey,
                                                  [0xA1] = &LDA_indirectx,
                                                  [0xB1] = &LDA_indirecty,

                                                  [0xA2] = &LDX_im,
                                                  [0xA6] = &LDX_zero,
                                                  [0xB6] = &LDX_zeroy,
                                                  [0xAE] = &LDX_absolute,
                                                  [0xBE] = &LDX_absolutey,
                                                  [0xA0] = &LDY_im,
                                                  [0xA4] = &LDY_zero,
                                                  [0xB4] = &LDY_zerox,
                                                  [0xAC] = &LDY_absolute,
                                                  [0xBC] = &LDY_absolutex,

                                                  [0x4A] = &LSR_accumulator,
                                                  [0x46] = &LSR_zero,
                                                  [0x56] = &LSR_zerox,
                                                  [0x4E] = &LSR_absolute,
                                                  [0x5E] = &LSR_absolutex,

                                                  [0xEA] = &NOP,

                                                  [0x05] = &ORA_im,
                                                  [0x09] = &ORA_zero,
                                                  [0x15] = &ORA_zerox,
                                                  [0x0D] = &ORA_absolute,
                                                  [0x1D] = &ORA_absolutex,
                                                  [0x19] = &ORA_absolutey,
                                                  [0x01] = &ORA_indirectx,
                                                  [0x11] = &ORA_indirecty,

                                                  [0x48] = &PHA,
                                                  [0x08] = &PHP,
                                                  [0x68] = &PLA,
                                                  [0x28] = &PLP,

                                                  [0x2A] = &ROL_accumulator,
                                                  [0x26] = &ROL_zero,
                                                  [0x36] = &ROL_zerox,
                                                  [0x2e] = &ROL_absolute,
                                                  [0x3e] = &ROL_absolutex,

                                                  [0x6a] = &ROR_accumulator,
                                                  [0x66] = &ROR_zero,
                                                  [0x76] = &ROR_zerox,
                                                  [0x6e] = &ROR_absolute,
                                                  [0x7e] = &ROR_absolutex,

                                                  [0x40] = &RTI,
                                                  [0x60] = &RTS,
                                                  [0xE9] = &SBC_im,
                                                  [0xE5] = &SBC_zero,
                                                  [0xF5] = &SBC_zerox,
                                                  [0xED] = &SBC_absolute,
                                                  [0xFD] = &SBC_absolutex,
                                                  [0xF9] = &SBC_absolutey,
                                                  [0xE1] = &SBC_indirectx,
                                                  [0xF1] = &SBC_indirecty,

                                                  [0x38] = &SEC,
                                                  [0xF8] = &SED,
                                                  [0x78] = &SEI,

                                                  [0x85] = &STA_zero,
                                                  [0x95] = &STA_zerox,
                                                  [0x8d] = &STA_absolute,
                                                  [0x9d] = &STA_absolutex,
                                                  [0x99] = &STA_absolutey,
                                                  [0x81] = &STA_indirectx,
                                                  [0x91] = &STA_indirecty,
                                                  [0x86] = &STX_zero,
                                                  [0x96] = &STX_zeroy,
                                                  [0x8e] = &STX_absolute,
                                                  [0x84] = &STY_zero,
                                                  [0x94] = &STY_zerox,
                                                  [0x8c] = &STY_absolute,

                                                  [0xaa] = &TAX,
                                                  [0xa8] = &TAY,
                                                  [0xba] = &TSX,
                                                  [0x8a] = &TXA,
                                                  [0x9a] = &TXS,
                                                  [0x98] = &TYA};
  unsigned char opcode = read_byte();

  if (instructions[opcode]) {
    instructions[opcode]();
  } else {
    printf("invalid opcode: %X\n", opcode);
    printf("PC: %x\n", registers->pc);
  }
}

static inline void push_to_stack(unsigned char value) {
  registers->stack_pointer[registers->_sp++] = value;
}

static inline unsigned char get_from_stack(void) {
  return registers->stack_pointer[registers->_sp--];
}

static inline unsigned char peek_from_stack(void) {
  return registers->stack_pointer[registers->_sp];
}

static uint8_t *indexed_indirect(uint8_t address) {
  uint16_t _location = read_word_at(address);
  uint16_t location = read_byte_at(_location + registers->x);
  return &memory[location];
}

static inline uint8_t *indirect_indexed(uint8_t address) {
  uint16_t _location = read_word_at(address);
  uint16_t location = read_byte_at(_location);
  return &memory[location + registers->y];
}

static inline void set_flag(flags_t flag, bool b) {
  if (get_flag(flag) != b) {
    registers->status ^= flag;
  }
}

static void set_flags(flags_t *flags, int n) {
  /* this will be left in for now, might be taken back into use later */
  for (int i = 0; i < n; i++) {
    switch (flags[i]) {
    case OVERFLOW:
      break;
    case ZERO:
      if (registers->accumulator == 0)
        set_flag(ZERO, true);
      break;
    case INTERRUPT:
      /* TODO */
      break;
    case DECIMAL:
      /* TODO */
      break;
    case CARRY:
      if (registers->accumulator > CHAR_MAX)
        set_flag(OVERFLOW, true);
      break;
    case BREAK:
      break;
    case NEGATIVE:
      break;
    }
  }
}

static inline uint8_t get_flag(flags_t flag) {
  /* !! to turn it into a 1 or 0 to not require shifting and
     looking up the power of two it is */
  return !!(registers->status & flag);
}

static inline uint16_t read_word() {
  uint16_t value = ((memory[registers->pc + 1] << 8) | memory[registers->pc]);
  registers->pc += 2;
  return value;
}

static inline uint16_t read_word_at(uint16_t location) {
  uint16_t value = (memory[location + 1] << 8) | memory[location];
  return value;
}

static inline uint8_t read_byte_at(uint16_t location) {
  uint8_t value = memory[location];
  return value;
}
static inline uint8_t read_byte() {
  uint8_t value = memory[registers->pc++];
  return value;
}

static inline void write_byte(uint8_t value, uint16_t location) {
  memory[location] = value;
}

static inline void zero_check(uint8_t value) {
  if (!value) {
    set_flag(ZERO, true);
  } else {
    set_flag(ZERO, false);
  }
}
static inline void negative_check(uint8_t value) {
  if (value & 0x80) {
    set_flag(NEGATIVE, true);
  } else {
    set_flag(NEGATIVE, false);
  }
}
static inline void overflow_check(uint16_t value, uint16_t result) {
  if ((result ^ registers->accumulator) & (result ^ value) & 0x80) {
    set_flag(OVERFLOW, true);
  } else {
    set_flag(OVERFLOW, false);
  }
}

static inline void carry_check(uint16_t value) {
  if (value & 0xff00) {
    set_flag(CARRY, true);
  } else {
    set_flag(CARRY, false);
  }
}

void ADC_help(uint16_t value) {
  uint16_t result = registers->accumulator + value + get_flag(CARRY);
  overflow_check(value, result);
  carry_check(result);
  zero_check(result);
  negative_check(result);
  registers->accumulator = result;
}

void ADC_im(void) {
  uint8_t value = read_byte();
  ADC_help(value);
}

void ADC_zero(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location]);
}

void ADC_zerox(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location + registers->x]);
}

void ADC_zeroy(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location + registers->y]);
}

void ADC_absolute(void) {
  uint16_t location = read_word();
  ADC_help(memory[location]);
}

void ADC_absolutex(void) {
  uint16_t location = read_word();
  ADC_help(memory[location + registers->x]);
}

void ADC_absolutey(void) {
  uint16_t location = read_word();
  ADC_help(memory[location + registers->y]);
}

void ADC_indirectx(void) {
  uint8_t location = read_byte();
  uint16_t value = read_word_at(location + registers->x);
  printf("%d\n", value);
  ADC_help(value);
}

void ADC_indirecty(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_word_at(_location + registers->y);
  uint16_t value = read_word_at(location);

  ADC_help(value);
}

void AND_help(uint8_t value) {
  registers->accumulator &= value;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}
void AND_im(void) { AND_help(read_byte()); }

void AND_zero(void) { AND_help(memory[read_byte()]); }

void AND_zerox(void) { AND_help(memory[read_byte() + registers->x]); }

void AND_absolute(void) { AND_help(memory[read_word()]); }

void AND_absolutex(void) { AND_help(memory[read_word() + registers->x]); }

void AND_absolutey(void) { AND_help(memory[read_word() + registers->y]); }

void AND_indirectx(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location + registers->x);
  uint8_t value = read_byte_at(location);
  AND_help(value);
}

void AND_indirecty(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location);
  uint8_t value = read_byte_at(location + registers->y);
}

void ASL_help(uint8_t *address) {
  *address <<= 1;
  zero_check(*address);
  carry_check(*address);
  negative_check(*address);
}

void ASL_accumulator(void) { ASL_help(&registers->accumulator); }

void ASL_zero(void) { ASL_help(&memory[read_byte()]); }

void ASL_zerox(void) { ASL_help(&memory[read_byte() + registers->x]); }

void ASL_absolute(void) { ASL_help(&memory[read_word()]); }

void ASL_absolutex(void) { ASL_help(&memory[read_word() + registers->x]); }

void BCC(void) {
  int8_t offset = read_byte();
  if (!get_flag(CARRY)) {
    registers->pc += offset;
  }
}

void BCS(void) {
  int8_t offset = read_byte();
  if (get_flag(CARRY)) {
    registers->pc += offset;
  }
}

void BEQ(void) {
  int8_t offset = read_byte();
  if (get_flag(ZERO)) {
    registers->pc += offset;
  }
}

void BIT_help(uint16_t bit_test) {
  if (bit_test == 0) {
    set_flag(ZERO, true);
    set_flag(OVERFLOW, false);
    set_flag(NEGATIVE, false);
  } else {
    set_flag(OVERFLOW, !!(bit_test & OVERFLOW));
    set_flag(NEGATIVE, !!(bit_test & NEGATIVE));
  }
}

void BIT_zero(void) {
  uint8_t location = read_byte();
  uint8_t bit_test = registers->accumulator & memory[location];
  BIT_help(bit_test);
}

void BIT_absolute(void) {
  uint16_t location = read_word();
  uint8_t bit_test = registers->accumulator & memory[location];
  BIT_help(bit_test);
}

void BMI(void) {
  int8_t offset = read_byte();
  if (get_flag(NEGATIVE)) {
    registers->pc += offset;
  }
}

void BNE(void) {
  int8_t offset = read_byte();
  if (!get_flag(ZERO)) {
    registers->pc += offset;
  }
}

void BPL(void) {
  int8_t offset = read_byte();
  if (!get_flag(NEGATIVE)) {
    registers->pc += offset;
  }
}

void BRK(void) {
  push_to_stack(registers->pc);
  push_to_stack(registers->status);
  set_flag(BREAK, true);
  registers->pc = 0xFFFE;
}

void BVC(void) {
  int8_t offset = read_byte();
  if (!get_flag(OVERFLOW)) {
    registers->pc += offset;
  }
}
void BVS(void) {
  int8_t offset = read_byte();
  if (get_flag(OVERFLOW)) {
    registers->pc += offset;
  }
}

void CLC(void) { set_flag(CARRY, false); }

void CLD(void) { set_flag(DECIMAL, false); }

void CLI(void) { set_flag(INTERRUPT, false); }

void CLV(void) { set_flag(OVERFLOW, false); }

/* this is for all compare instructions */
void cmp_help(unsigned char value, unsigned char reg) {
  if (reg == value) {
    set_flag(ZERO, true);
  } else if (reg >= value) {
    set_flag(CARRY, true);
  }

  if ((signed char)(reg - value) < 0) {
    set_flag(NEGATIVE, true);
  }
}

void CMP_help(unsigned char value) { cmp_help(value, registers->accumulator); }

void CMP_im(void) {
  unsigned char value = read_byte();
  CMP_help(value);
}

void CMP_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = memory[location];
  CMP_help(value);
}

void CMP_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = memory[location + registers->x];
  CMP_help(value);
}

void CMP_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = memory[location];
  CMP_help(value);
}

void CMP_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = memory[location + registers->x];
  CMP_help(value);
}

void CMP_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  CMP_help(value);
}

void CMP_indirectx(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location + registers->x);
  uint8_t value = read_byte_at(location);
  CMP_help(value);
}

void CMP_indirecty(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location);
  uint8_t value = read_byte_at(location + registers->y);
  CMP_help(value);
}

void CPX_help(unsigned char value) { cmp_help(value, registers->x); }

void CPX_im(void) {
  uint8_t value = read_byte();
  CPX_help(value);
}

void CPX_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

void CPX_absolute(void) {
  uint8_t location = read_word();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

void CPY_help(unsigned char value) { cmp_help(value, registers->y); }

void CPY_im(void) {
  uint8_t value = read_byte();
  CPX_help(value);
}

void CPY_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

void CPY_absolute(void) {
  uint8_t location = read_word();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

void DEC_help(uint16_t location) {
  memory[location]--;
  zero_check(memory[location]);
  negative_check(memory[location]);
}

void DEC_zero(void) {
  uint8_t location = read_byte();
  DEC_help(location);
}

void DEC_zerox(void) {
  uint8_t location = read_byte();
  DEC_help(location);
}

void DEC_absolute(void) {
  uint16_t location = read_word();
  DEC_help(location);
}

void DEC_absolutex(void) {
  uint16_t location = read_word();
  DEC_help(location + registers->x);
}

void DEX(void) {
  registers->x--;
  zero_check(registers->x);
  negative_check(registers->x);
}

void DEY(void) {
  registers->y--;
  zero_check(registers->y);
  negative_check(registers->y);
}

void EOR_help(uint8_t value) {
  registers->accumulator ^= value;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

void EOR_im(void) {
  uint8_t value = read_byte();
  EOR_help(value);
}

void EOR_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  EOR_help(value);
}

void EOR_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  EOR_help(value);
}

void EOR_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  EOR_help(value);
}

void EOR_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->x);
  EOR_help(value);
}

void EOR_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  EOR_help(value);
}

void EOR_indirectx(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_byte_at(_location + registers->x);
  uint16_t value = read_word_at(location);
  EOR_help(value);
}

void EOR_indirecty(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_byte_at(_location);
  uint16_t value = read_word_at(location);
  EOR_help(value);
}

void INC_help(uint16_t location) {
  flags_t affects[] = {ZERO, NEGATIVE};
  memory[location]++;
  zero_check(memory[location]);
  negative_check(memory[location]);
}

void INC_zero(void) {
  uint8_t location = read_byte();
  INC_help(location);
}

void INC_zerox(void) {
  uint8_t location = read_byte();
  INC_help(location);
}

void INC_absolute(void) {
  uint8_t location = read_word();
  INC_help(location);
}

void INC_absolutex(void) {
  uint8_t location = read_word();
  INC_help(location);
}

void INX(void) {
  registers->x++;
  zero_check(registers->x);
  negative_check(registers->x);
}

void INY(void) {
  registers->y++;
  zero_check(registers->y);
  negative_check(registers->y);
}

void JMP_absolute(void) { registers->pc = read_word(); }

void JMP_indirect(void) {
  uint16_t location = read_word();
  uint16_t jmp_to = read_word_at(location);
  registers->pc = jmp_to;
}

void JSR(void) {
  registers->stack_pointer[registers->_sp] = registers->pc - 1;
  registers->pc = read_word();
}

void LDA_help(uint16_t value) {
  registers->accumulator = value;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

void LDA_im(void) {
  uint8_t value = read_byte();
  printf("LDA value: %x\n", value);
  LDA_help(value);
}

void LDA_zero(void) { LDA_help(memory[read_byte()]); }

void LDA_zerox(void) { LDA_help(memory[read_byte() + registers->x]); }

void LDA_absolute(void) { LDA_help(memory[read_word()]); }

void LDA_absolutex(void) { LDA_help(memory[read_word() + registers->x]); }

void LDA_absolutey(void) { LDA_help(memory[read_word() + registers->y]); }

void LDA_indirectx(void) {
  uint8_t _location = read_byte() + registers->x;
  uint16_t location = read_word_at(_location);
  uint16_t value = read_word_at(location);
  printf("LDA indirect location: %x, value: %x\n", location, value);
  LDA_help(value);
}

void LDA_indirecty(void) {
  uint8_t _location = read_byte() + registers->y;
  uint16_t location = read_word_at(_location);
  uint16_t value = read_word_at(location);
  printf("LDA value: %x\n", value);
  LDA_help(value);
}

void LDX_help(uint8_t value) {
  registers->x = value;
  zero_check(registers->x);
  negative_check(registers->x);
}

void LDX_im(void) {
  uint8_t value = read_byte();
  printf("ldx value: %x\n", value);
  LDX_help(value);
}

void LDX_zero(void) { LDX_help(memory[read_byte()]); }

void LDX_zeroy(void) { LDX_help(memory[read_byte() + registers->y]); }

void LDX_absolute(void) { LDX_help(memory[read_word()]); }

void LDX_absolutey(void) { LDX_help(memory[read_word() + registers->y]); }

void LDY_help(uint8_t value) {
  registers->y = value;
  zero_check(registers->y);
  negative_check(registers->y);
}

void LDY_im(void) { LDY_help(read_byte()); }

void LDY_zero(void) { LDY_help(memory[read_byte()]); }

void LDY_zerox(void) { LDY_help(memory[read_byte() + registers->x]); }

void LDY_absolute(void) { LDY_help(memory[read_word()]); }

void LDY_absolutex(void) { LDY_help(memory[read_word() + registers->y]); }

void LSR_help(uint8_t *value) {
  flags_t affects[] = {ZERO, CARRY, NEGATIVE};
  *value >>= 1;
  carry_check(*value);
  zero_check(*value);
  negative_check(*value);
}

void LSR_accumulator(void) { LSR_help(&registers->accumulator); }

void LSR_zero(void) {
  uint8_t location = read_byte();
  LSR_help(&memory[location]);
}

void LSR_zerox(void) {
  uint8_t location = read_byte();
  LSR_help(&memory[location + registers->x]);
}

void LSR_absolute(void) {
  uint16_t location = read_word();
  LSR_help(&memory[location]);
}

void LSR_absolutex(void) {
  uint16_t location = read_word();
  LSR_help(&memory[location + registers->x]);
}

void NOP(void) { registers->pc++; }

void ORA_help(uint16_t value) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers->accumulator |= read_byte();
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

void ORA_im(void) {
  uint8_t value = read_byte();
  ORA_help(value);
}

void ORA_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

void ORA_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

void ORA_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

void ORA_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->x);
  ORA_help(value);
}

void ORA_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  ORA_help(value);
}

void ORA_indirectx(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_word_at(_location + registers->x);
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

void ORA_indirecty(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_word_at(_location);
  uint8_t value = read_byte_at(location + registers->y);
  ORA_help(value);
}

void PHA(void) { push_to_stack(read_byte()); }

void PHP(void) { push_to_stack(registers->status); }

void PLA(void) { registers->accumulator = get_from_stack(); }

void PLP(void) { registers->status = get_from_stack(); }

void ROL_help(uint8_t *location) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  *location = (*location << 1) | (*location >> 7);
  carry_check(*location);
  zero_check(*location);
  negative_check(*location);
}

void ROL_accumulator(void) { ROL_help(&registers->accumulator); }

void ROL_zero(void) {
  uint8_t location = read_byte();
  ROL_help(&memory[location]);
}

void ROL_zerox(void) {
  uint8_t location = read_byte();
  ROL_help(&memory[location + registers->x]);
}

void ROL_absolute(void) {
  uint16_t location = read_word();
  ROL_help(&memory[location]);
}

void ROL_absolutex(void) {
  uint16_t location = read_word();
  ROL_help(&memory[location + registers->x]);
}

void ROR_help(uint8_t *location) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  *location = (*location >> 1) | (*location << 7);
  carry_check(*location);
  zero_check(*location);
  negative_check(*location);
}

void ROR_accumulator(void) { ROR_help(&registers->accumulator); }

void ROR_zero(void) {
  uint8_t location = read_byte();
  ROR_help(&memory[location]);
}

void ROR_zerox(void) {
  uint8_t location = read_byte();
  ROR_help(&memory[location + registers->x]);
}

void ROR_absolute(void) {
  uint16_t location = read_word();
  ROR_help(&memory[location]);
}

void ROR_absolutex(void) {
  uint16_t location = read_word();
  ROR_help(&memory[location + registers->x]);
}

void RTI(void) {
  registers->status = get_from_stack();
  registers->pc++;
}

void RTS(void) {
  registers->pc = get_from_stack();
  registers->pc++;
}

void SBC_help(uint8_t amt) {
  flags_t affects[] = {OVERFLOW, ZERO, NEGATIVE, CARRY};
  uint16_t result = registers->accumulator - amt - get_flag(CARRY);
  overflow_check(amt, result);
  carry_check(result);
  zero_check(result);
  negative_check(result);
  registers->accumulator = result;
}

void SBC_im(void) {
  uint8_t value = read_byte();
  SBC_help(value);
}

void SBC_zero(void) {
  uint8_t location = read_byte();
  SBC_help(read_byte_at(location));
}

void SBC_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  SBC_help(value);
}

void SBC_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  SBC_help(value);
}

void SBC_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->x);
  SBC_help(value);
}

void SBC_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  SBC_help(value);
}

void SBC_indirectx(void) {
  uint16_t address = read_word();
  uint8_t *ptr = indexed_indirect(address);
  SBC_help(*ptr);
}

void SBC_indirecty(void) {
  uint16_t address = read_word();
  uint8_t *ptr = indirect_indexed(address);
  SBC_help(*ptr);
}

void SEC(void) { set_flag(CARRY, true); }
void SED(void) { set_flag(DECIMAL, true); }
void SEI(void) { set_flag(INTERRUPT, true); }

void STA_zero(void) {
  uint8_t location = read_byte();
  write_byte(registers->accumulator, location);
}

void STA_zerox(void) {
  uint8_t address = read_byte();
  write_byte(registers->accumulator, address + registers->x);
}

void STA_absolute(void) {
  uint16_t address = read_word();
  write_byte(registers->accumulator, address);
}

void STA_absolutex(void) {
  uint16_t address = read_word();
  write_byte(registers->accumulator, address + registers->x);
}

void STA_absolutey(void) {
  uint16_t address = read_word();
  write_byte(registers->accumulator, address + registers->y);
}

void STA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *ptr = indexed_indirect(address);
  *ptr = registers->accumulator;
}

void STA_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t *ptr = indirect_indexed(address);
  *ptr = registers->accumulator;
}

void STX_zero(void) { write_byte(registers->x, read_byte()); }
void STX_zeroy(void) { write_byte(registers->x, read_byte()); }
void STX_absolute(void) { write_byte(registers->x, read_word()); }

void STY_zero(void) { write_byte(registers->y, read_byte()); }
void STY_zerox(void) { write_byte(registers->y, read_byte() + registers->x); }
void STY_absolute(void) {
  write_byte(registers->y, read_word() + registers->x);
}

void TAX(void) {
  registers->x = registers->accumulator;
  zero_check(registers->x);
  negative_check(registers->x);
}

void TAY(void) {
  registers->y = registers->accumulator;
  zero_check(registers->y);
  negative_check(registers->y);
}
void TSX(void) {
  registers->x = peek_from_stack();
  zero_check(registers->x);
  negative_check(registers->x);
}

void TXA(void) {
  registers->accumulator = peek_from_stack();
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}
void TXS(void) {
  flags_t affects[] = {ZERO, NEGATIVE};
  push_to_stack(registers->x);
  set_flags(affects, 2);
}

void TYA(void) {
  registers->accumulator = registers->y;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}
