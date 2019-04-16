#include "../headers/cpu.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unordered_map>

#ifdef DEBUG
#define __print_addressingmode(v) printf("%s", (v))
#else
#define __print_addressingmode(v)
#endif

/* TODO:
 * 1. make it so you can just make a list of flags to check for and it sets them
 * 2. maybe remove addressing mode from instructions and move calculation to a
 * separate function where it reads
 * everything etc
 * 3. maybe move set_flags from per instruction to each instruction returns a
 * list of affected registers and call set_flags from interpret_opcode
 */

typedef void (*instruction_pointer)(addressing_modes_t);

static processor_registers registers;
/* for "easier" access to the different parts of memory */
static memory_map _memory;
static unsigned char *memory = (unsigned char *)&_memory;

/* this will be rewritten once the cpu is done */
extern void initialize_cpu(const unsigned char *data, size_t size) {
  registers._sp = 0;
  registers.accumulator = 0;
  registers.pc = 0x600;
  registers.status = 0;
  registers.x = 0;
  registers.y = 0;

  memcpy(&(memory[0x600]), data, size);
}

extern void interpret_opcode(void) {
  /* figure out how to reduce the amount of shit in this? */
  static std::unordered_map<unsigned char, void (*)(void)> instructions = {
      {0x69, &ADC_im},        {0x65, &ADC_zero},      {0x75, &ADC_zerox},
      {0x6D, &ADC_absolute},  {0x7D, &ADC_absolutex}, {0x79, &ADC_absolutey},
      {0x61, &ADC_indirectx}, {0x71, &ADC_indirecty},

      {0x29, &AND},           {0x25, &AND},           {0x35, &AND},
      {0x2D, &AND},           {0x3D, &AND},           {0x39, &AND},
      {0x21, &AND},           {0x31, &AND},

      {0x0A, &ASL},           {0x06, &ASL},           {0x16, &ASL},
      {0x0E, &ASL},           {0x1E, &ASL},

      {0x90, &BCC},           {0xB0, &BCS},           {0xF0, &BEQ},

      {0x24, &BIT},           {0x2C, &BIT},

      {0x30, &BMI},           {0xD0, &BNE},           {0x10, &BPL},
      {0x00, &BRK},           {0x50, &BVC},           {0x70, &BVS},

      {0x18, &CLC},           {0xD8, &CLD},           {0x58, &CLI},
      {0xB8, &CLV},

      {0xC9, &CMP},           {0xC5, &CMP},           {0xD5, &CMP},
      {0xCD, &CMP},           {0xDD, &CMP},           {0xD9, &CMP},
      {0xC1, &CMP},           {0xD1, &CMP},

      {0xE0, &CPX},           {0xE4, &CPX},           {0xEC, &CPX},
      {0xC0, &CPY},           {0xC4, &CPY},           {0xCC, &CPY},

      {0xC6, &DEC},           {0xD6, &DEC},           {0xCE, &DEC},
      {0xDE, &DEC},           {0xCA, &DEX},           {0x88, &DEY},

      {0x49, &EOR},           {0x45, &EOR},           {0x55, &EOR},
      {0x4D, &EOR},           {0x5D, &EOR},           {0x59, &EOR},
      {0x41, &EOR},           {0x51, &EOR},

      {0xE6, &INC},           {0xF6, &INC},           {0xEE, &INC},
      {0xFE, &INC},           {0xE8, &INX},           {0xC8, &INY},
      {0x4C, &JMP},           {0x6C, &JMP},           {0x20, &JSR},

      {0xA9, &LDA_im},        {0xA5, &LDA_zero},      {0xB5, &LDA_zerox},
      {0xAD, &LDA_absolute},  {0xBD, &LDA_absolutex}, {0xB9, &LDA_absolutey},
      {0xA1, &LDA_indirectx}, {0xB1, &LDA_indirecty},

      {0xA2, &LDX_im},        {0xA6, &LDX_zero},      {0xB6, &LDX_zeroy},
      {0xAE, &LDX_absolute},  {0xBE, &LDX_absolutey}, {0xA0, &LDY_im},
      {0xA4, &LDY_zero},      {0xB4, &LDY_zerox},     {0xAC, &LDY_absolute},
      {0xBC, &LDY_absolutex},

      {0x4A, &LSR},           {0x46, &LSR},           {0x56, &LSR},
      {0x4E, &LSR},           {0x5E, &LSR},

      {0xEA, &NOP},

      {0x05, &ORA},           {0x09, &ORA},           {0x15, &ORA},
      {0x0D, &ORA},           {0x1D, &ORA},           {0x19, &ORA},
      {0x01, &ORA},           {0x11, &ORA},

      {0x48, &PHA},           {0x08, &PHP},           {0x69, &PLA},
      {0x28, &PLP},

      {0x2A, &ROL},           {0x26, &ROL},           {0x36, &ROL},
      {0x2e, &ROL},           {0x3e, &ROL},

      {0x6a, &ROR},           {0x66, &ROR},           {0x76, &ROR},
      {0x6e, &ROR},           {0x7e, &ROR},

      {0x40, &RTI},           {0x60, &RTS},           {0xE9, &SBC},
      {0xE5, &SBC},           {0xF5, &SBC},           {0xED, &SBC},
      {0xFD, &SBC},           {0xF9, &SBC},           {0xE1, &SBC},
      {0xF1, &SBC},

      {0x38, &SEC},           {0xF8, &SED},           {0x78, &SEI},
      {0x85, &STA},           {0x86, &STX},           {0x8c, &STY}};

  unsigned char opcode = read_byte();
  addressing_modes_t mode = decode_addressing_mode(opcode);
  instructions[opcode]();
}

static inline void push_to_stack(unsigned char value) {
  registers.stack_pointer[registers._sp++] = value;
}

static inline unsigned char get_from_stack(void) {
  return registers.stack_pointer[registers._sp--];
}

static inline unsigned char peek_from_stack(void) {
  return registers.stack_pointer[registers._sp];
}

static inline void set_flag(flags_t flag, bool b) {
  if (get_flag(flag) != b) {
    registers.status ^= flag;
  }
}

static void set_flags(flags_t *flags, int n) {
  for (int i = 0; i < n; i++) {
    switch (flags[i]) {
    case OVERFLOW:
      break;
    case ZERO:
      if (registers.accumulator == 0)
        set_flag(ZERO, true);
      break;
    case INTERRUPT:
      /* TODO */
      break;
    case DECIMAL:
      /* TODO */
      break;
    case CARRY:
      if (registers.accumulator > CHAR_MAX)
        set_flag(OVERFLOW, true);
      break;
    case BFLAG:
      break;
    case NEGATIVE:
      /* TODO */
      break;
    }
  }
}

static inline uint8_t get_flag(flags_t flag) {
  return !!(registers.status &
            flag); /* !! to turn it into a 1 or 0 to not require shifting and
                      looking up the power of two it is */
}

static inline uint16_t read_word() {
  uint16_t value = ((memory[registers.pc + 1] << 8) | memory[registers.pc]);
  registers.pc += 2;
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
  uint8_t value = memory[registers.pc++];
  return value;
}

static inline void write_byte(uint8_t value, uint16_t location) {
  memory[location] = value;
}

static addressing_modes_t decode_addressing_mode(uint8_t opcode) {
  static std::unordered_map<int, addressing_modes_t> modes10 = {
      {0x0, IMMEDIATE}, {0x1, ZERO_PAGE},   {0x2, ACCUMULATOR},
      {0x3, ABSOLUTE},  {0x5, ZERO_PAGE_X}, {0x7, ABSOLUTE_X}};
  static std::unordered_map<int, addressing_modes_t> modes01 = {
      {0x0, INDIRECT_X}, {0x1, ZERO_PAGE},  {0x2, IMMEDIATE},
      {0x3, ABSOLUTE},   {0x4, INDIRECT_Y}, {0x5, ZERO_PAGE_X},
      {0x6, ABSOLUTE_Y}, {0x7, ABSOLUTE_X}};
  static std::unordered_map<int, addressing_modes_t> modes00 = {
      {0x0, IMMEDIATE},
      {0x1, ZERO_PAGE},
      {0x3, ABSOLUTE},
      {0x5, ZERO_PAGE_X},
      {0x7, ABSOLUTE_X}};
  const uint8_t type_mask = 0x3;
  const uint8_t mode_mask = 0x1c;
  /* this is done to check which type of opcode
   * we're dealing with as there's 4 different ways to decode it*/
  const uint8_t type = opcode & type_mask;
  const uint8_t mode = (opcode & mode_mask) >> 2;

  /* TODO make rules for when you're trying to get access to an addressing mode
   * that doesn't exist for the given instruction for now it assumes everything
   * has a respective addressing mode */
  switch (type) {
  case 0x0:
    return modes00[mode];
  case 0x1:
    return modes01[mode];
  case 0x2:
    return modes10[mode];
  default:
    return UNKNOWN;
  }
}

static const char *print_addressingmode(addressing_modes_t mode) {
  const char *const modes[] = {
      "IMPLICT",     "ACCUMULATOR", "IMMEDIATE",  "ZERO_PAGE",  "ZERO_PAGE_X",
      "ZERO_PAGE_Y", "RELATIVE",    "IMPLIED",    "ABSOLUTE",   "ABSOLUTE_X",
      "ABSOLUTE_Y",  "INDIRECT",    "INDIRECT_X", "INDIRECT_Y", "UNKNOWN"};
  __print_addressingmode(modes[mode]);
  return modes[mode];
}

/* TODO add tests for this and decode addressing mode */
static bool compare(uint8_t opcode) {
  uint8_t compare_to = (opcode & 0x20) >> 5;
  /* we need to get the first two bytes which decides which register to compare
   */
  switch ((opcode & 0xc0) >> 6) {
  case 0x0:
    return get_flag(NEGATIVE) == compare_to;
  case 0x1:
    return get_flag(OVERFLOW) == compare_to;
  case 0x2:
    return get_flag(CARRY) == compare_to;
  case 0x3:
    return get_flag(ZERO) == compare_to;
  }
  return false;
}

void ADC_help(uint16_t value) {
  flags_t affected[] = {CARRY, ZERO, OVERFLOW, NEGATIVE};
  registers.accumulator += value + get_flag(CARRY);
  set_flags(affected, 4);
}

/* ADC start */
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
  ADC_help(memory[location + registers.x]);
}

void ADC_zeroy(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location + registers.y]);
}

void ADC_absolute(void) {
  uint16_t location = read_word();
  ADC_help(memory[location]);
}

void ADC_absolutex(void) {
  uint16_t location = read_word();
  ADC_help(memory[location + registers.x]);
}

void ADC_absolutey(void) {
  uint16_t location = read_word();
  ADC_help(memory[location + registers.y]);
}

void ADC_indirectx(void) {
  uint8_t location = read_byte();
  uint16_t value = read_word_at(location + registers.x);
  printf("%d\n", value);
  ADC_help(value);
}

void ADC_indirecty(void) {
  uint8_t location = read_byte();
  uint16_t value = read_word_at(location + registers.y);

  ADC_help(value);
}
/* end of ADC */

void AND(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.accumulator &= read_byte();
  set_flags(affected, 2);
}

void ASL(void) {
  flags_t affected[] = {ZERO, CARRY, NEGATIVE};
  registers.accumulator <<= 1;
  set_flags(affected, 3);
}

void BCC(void) {}
void BCS(void) {}
void BEQ(void) {}
void BIT(void) {}
void BMI(void) {}
void BNE(void) {}
void BPL(void) {}
void BRK(void) {}
void BVC(void) {}
void BVS(void) {}

void CLC(void) { set_flag(CARRY, false); }

void CLD(void) { set_flag(DECIMAL, false); }

void CLI(void) { set_flag(INTERRUPT, false); }

void CLV(void) { set_flag(OVERFLOW, false); }

void CMP(void) {
  /* TODO */
  unsigned char compare_to = read_byte();
}
void CPX(void) {}
void CPY(void) {}

void DEC(void) {}

void DEX(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.x--;
  set_flags(affected, 2);
}

void DEY(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.y--;
  set_flags(affected, 2);
}

void EOR(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.accumulator ^= read_byte();
  set_flags(affected, 2);
}

void INC(void) {}
void INX(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.x++;
  set_flags(affected, 2);
}

void INY(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.x++;
  set_flags(affected, 2);
}

void JMP(void) { registers.pc = read_word(); }

void JSR(void) {
  registers.stack_pointer[registers._sp] = registers.pc - 1;
  registers.pc = read_word();
}

void LDA_help(uint16_t value) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.accumulator = value;
  set_flags(affected, 2);
}

void LDA_im(void) {
  uint8_t value = read_byte();
  printf("LDA value: %x\n", value);
  LDA_help(value);
}

void LDA_zero(void) { LDA_help(memory[read_byte()]); }

void LDA_zerox(void) { LDA_help(memory[read_byte() + registers.x]); }

void LDA_absolute(void) { LDA_help(memory[read_word()]); }

void LDA_absolutex(void) { LDA_help(memory[read_word() + registers.x]); }

void LDA_absolutey(void) { LDA_help(memory[read_word() + registers.y]); }

void LDA_indirectx(void) {
  uint8_t _location = read_byte() + registers.x;
  uint16_t location = read_word_at(_location);
  uint16_t value = read_word_at(location);
  printf("LDA indirect location: %x, value: %x\n", location, value);
  LDA_help(value);
}

void LDA_indirecty(void) {
  uint8_t _location = read_byte() + registers.y;
  uint16_t location = read_word_at(_location);
  uint16_t value = read_word_at(location);
  printf("LDA value: %x\n", value);
  LDA_help(value);
}

void LDX_help(uint8_t value) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.x = value;
  set_flags(affected, 2);
}
void LDX_im(void) {
  uint8_t value = read_byte();
  printf("ldx value: %x\n", value);
  LDX_help(value);
}

void LDX_zero(void) { LDX_help(memory[read_byte()]); }

void LDX_zeroy(void) { LDX_help(memory[read_byte() + registers.y]); }

void LDX_absolute(void) { LDX_help(memory[read_word()]); }

void LDX_absolutey(void) { LDX_help(memory[read_word() + registers.y]); }

void LDY_help(uint8_t value) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.y = value;
  set_flags(affected, 2);
}

void LDY_im(void) { LDY_help(read_byte()); }

void LDY_zero(void) { LDY_help(memory[read_byte()]); }

void LDY_zerox(void) { LDY_help(memory[read_byte() + registers.x]); }

void LDY_absolute(void) { LDY_help(memory[read_word()]); }

void LDY_absolutex(void) { LDY_help(memory[read_word() + registers.y]); }

void LSR(void) {}
void NOP(void) { registers.pc++; }
void ORA(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.accumulator |= read_byte();
  set_flags(affected, 2);
}

void PHA(void) { push_to_stack(read_byte()); }

void PHP(void) { push_to_stack(registers.status); }

void PLA(void) { registers.accumulator = get_from_stack(); }

void PLP(void) { registers.status = get_from_stack(); }

void ROL(void) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  registers.accumulator =
      (registers.accumulator << 1) | (registers.accumulator >> (7));
}

void ROR(void) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  registers.accumulator =
      (registers.accumulator >> 1) | (registers.accumulator << (7));
}

void RTI(void) {
  registers.status = get_from_stack();
  registers.pc++;
}

void RTS(void) {
  registers.pc = get_from_stack();
  registers.pc++;
}

void SBC(void) { registers.accumulator -= read_byte() - (1 - get_flag(CARRY)); }

void SEC(void) { set_flag(CARRY, true); }
void SED(void) { set_flag(DECIMAL, true); }
void SEI(void) { set_flag(INTERRUPT, true); }

void STA(void) {
  uint8_t location = read_byte();
  printf("STA location: %d\n", location);
  write_byte(registers.accumulator, location);
}

void STX(void) { write_byte(registers.x, read_byte()); }

void STY(void) {
  uint16_t value = read_word();
  printf("STY value: %x\n", value);
  write_byte(registers.y, value);
}

void TAX(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.x = registers.accumulator;
  set_flags(affected, 2);
}
void TAY(void) {
  flags_t affected[] = {ZERO, NEGATIVE};
  registers.y = registers.accumulator;
  set_flags(affected, 2);
}
void TSX(void) { registers.x = peek_from_stack(); }
void TXA(void) { registers.accumulator = peek_from_stack(); }
void TXS(void) { push_to_stack(registers.x); }
void TYA(void) { registers.accumulator = registers.y; }
