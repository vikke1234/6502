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

static void ADC_absolute(void);
static void ADC_absolutex(void);
static void ADC_absolutey(void);
static void ADC_help(uint16_t value);
static void ADC_im(void);
static void ADC_indirectx(void);
static void ADC_indirecty(void);
static void ADC_zero(void);
static void ADC_zerox(void);
static void ADC_zeroy(void);
static void ALR(void);
static void ANC(void);
static void AND_absolute(void);
static void AND_absolutex(void);
static void AND_absolutey(void);
static void AND_help(uint8_t value);
static void AND_im(void);
static void AND_indirectx(void);
static void AND_indirecty(void);
static void AND_zero(void);
static void AND_zerox(void);
static void ASL_absolute(void);
static void ASL_absolutex(void);
static void ASL_accumulator(void);
static void ASL_help(uint8_t *address);
static void ASL_zero(void);
static void ASL_zerox(void);
static void ASO_absolute(void);
static void ASO_absolutex(void);
static void ASO_absolutey(void);
static void ASO_indirectx(void);
static void ASO_indirecty(void);
static void ASO_zero(void);
static void ASO_zerox(void);
static void AXA_absolutey(void);
static void AXA_indirecty(void);
static void AXS_absolute(void);
static void AXS_absolutey(void);
static void AXS_indirectx(void);
static void AXS_indirecty(void);
static void AXS_zero(void);
static void AXS_zeroy(void);
static void BCC(void);
static void BCS(void);
static void BEQ(void);
static void BIT_absolute(void);
static void BIT_help(uint16_t bit_test);
static void BIT_zero(void);
static void BMI(void);
static void BNE(void);
static void BPL(void);
static void BRK(void);
static void BVC(void);
static void BVS(void);
static void CLC(void);
static void CLD(void);
static void CLI(void);
static void CLV(void);
static void CMP_absolute(void);
static void CMP_absolutex(void);
static void CMP_absolutey(void);
static void CMP_help(unsigned char value);
static void CMP_im(void);
static void CMP_indirectx(void);
static void CMP_indirecty(void);
static void CMP_zero(void);
static void CMP_zerox(void);
static void CPX_absolute(void);
static void CPX_help(unsigned char value);
static void CPX_im(void);
static void CPX_zero(void);
static void CPY_absolute(void);
static void CPY_help(unsigned char value);
static void CPY_im(void);
static void CPY_zero(void);
static void DCM_absolute(void);
static void DCM_absolutex(void);
static void DCM_absolutey(void);
static void DCM_help(uint8_t *address);
static void DCM_indirectx(void);
static void DCM_indirecty(void);
static void DCM_zero(void);
static void DCM_zerox(void);
static void DEC_absolute(void);
static void DEC_absolutex(void);
static void DEC_help(uint16_t location);
static void DEC_zero(void);
static void DEC_zerox(void);
static void DEX(void);
static void DEY(void);
static void EOR_absolute(void);
static void EOR_absolutex(void);
static void EOR_absolutey(void);
static void EOR_help(uint8_t value);
static void EOR_im(void);
static void EOR_indirectx(void);
static void EOR_indirecty(void);
static void EOR_zero(void);
static void EOR_zerox(void);
static void HLT(void);
static void INC_absolute(void);
static void INC_absolutex(void);
static void INC_help(uint16_t location);
static void INC_zero(void);
static void INC_zerox(void);
static void INS_absolute(void);
static void INS_absolutex(void);
static void INS_absolutey(void);
static void INS_help(uint8_t *address);
static void INS_indirectx(void);
static void INS_indirecty(void);
static void INS_zero(void);
static void INS_zerox(void);
static void INX(void);
static void INY(void);
static void JMP_absolute(void);
static void JMP_indirect(void);
static void JSR(void);
static void LAS(void);
static void LAX_absolute(void);
static void LAX_absolutey(void);
static void LAX_indirectx(void);
static void LAX_indirecty(void);
static void LAX_zero(void);
static void LAX_zeroy(void);
static void LDA_absolute(void);
static void LDA_absolutex(void);
static void LDA_absolutey(void);
static void LDA_help(uint16_t value);
static void LDA_im(void);
static void LDA_indirectx(void);
static void LDA_indirecty(void);
static void LDA_zero(void);
static void LDA_zerox(void);
static void LDX_absolute(void);
static void LDX_absolutey(void);
static void LDX_help(uint8_t value);
static void LDX_im(void);
static void LDX_zero(void);
static void LDX_zeroy(void);
static void LDY_absolute(void);
static void LDY_absolutex(void);
static void LDY_help(uint8_t value);
static void LDY_im(void);
static void LDY_zero(void);
static void LDY_zerox(void);
static void LSE_absolute(void);
static void LSE_absolutex(void);
static void LSE_absolutey(void);
static void LSE_indirectx(void);
static void LSE_indirecty(void);
static void LSE_zero(void);
static void LSE_zerox(void);
static void LSR_absolute(void);
static void LSR_absolutex(void);
static void LSR_accumulator(void);
static void LSR_help(uint8_t *value);
static void LSR_zero(void);
static void LSR_zerox(void);
static void NOP(void);
static void ORA_absolute(void);
static void ORA_absolutex(void);
static void ORA_absolutey(void);
static void ORA_help(uint16_t value);
static void ORA_im(void);
static void ORA_indirectx(void);
static void ORA_indirecty(void);
static void ORA_zero(void);
static void ORA_zerox(void);
static void PHA(void);
static void PHP(void);
static void PLA(void);
static void PLP(void);
static void RLA_absolute(void);
static void RLA_absolutex(void);
static void RLA_absolutey(void);
static void RLA_indirectx(void);
static void RLA_indirecty(void);
static void RLA_zero(void);
static void RLA_zerox(void);
static void ROL_absolute(void);
static void ROL_absolutex(void);
static void ROL_accumulator(void);
static void ROL_help(uint8_t *location);
static void ROL_zero(void);
static void ROL_zerox(void);
static void ROR_absolute(void);
static void ROR_absolutex(void);
static void ROR_accumulator(void);
static void ROR_help(uint8_t *location);
static void ROR_zero(void);
static void ROR_zerox(void);
static void RRA_absolute(void);
static void RRA_absolutex(void);
static void RRA_absolutey(void);
static void RRA_indirectx(void);
static void RRA_indirecty(void);
static void RRA_zero(void);
static void RRA_zerox(void);
static void RRA_zeroy(void);
static void RTI(void);
static void RTS(void);
static void SAY(void);
static void SBC_absolute(void);
static void SBC_absolutex(void);
static void SBC_absolutey(void);
static void SBC_help(uint8_t amt);
static void SBC_im(void);
static void SBC_indirectx(void);
static void SBC_indirecty(void);
static void SBC_zero(void);
static void SBC_zerox(void);
static void SEC(void);
static void SED(void);
static void SEI(void);
static void SKW(void);
static void STA_absolute(void);
static void STA_absolutex(void);
static void STA_absolutey(void);
static void STA_indirectx(void);
static void STA_indirecty(void);
static void STA_zero(void);
static void STA_zerox(void);
static void STX_absolute(void);
static void STX_zero(void);
static void STX_zeroy(void);
static void STY_absolute(void);
static void STY_zero(void);
static void STY_zerox(void);
static void TAS(void);
static void TAX(void);
static void TAY(void);
static void TSX(void);
static void TXA(void);
static void TXS(void);
static void TYA(void);
static void XAS(void);
static void cmp_help(unsigned char value, unsigned char reg);

static inline void carry_check(uint16_t value);
static inline uint8_t *dereference_address(uint8_t address);
static inline uint8_t get_flag(flags_t flag);
static inline unsigned char get_from_stack(void);
static inline uint8_t *indexed_indirect(uint8_t address);
static inline uint8_t *indirect_indexed(uint8_t address);
static inline void negative_check(uint8_t value);
static inline void overflow_check(uint16_t value, uint16_t result);
static inline unsigned char peek_from_stack(void);
static inline void push_to_stack(unsigned char value);
static inline uint8_t read_byte();
static inline uint8_t read_byte_at(uint16_t location);
static inline uint16_t read_word();
static inline uint16_t read_word_at(uint16_t location);
static inline void set_flag(flags_t flag, bool b);
static inline void write_byte(uint8_t value, uint16_t location);
static inline void zero_check(uint8_t value);

/* this will probably be rewritten once the cpu is done and correct file
 * handling is made*/
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
  unsigned long long clock_ticks = 0;
  memcpy(&memory[registers->pc], &data[0x600], size - 0x600);
}

/* parser, pass data to initialize cpu and this does the rest */
extern void interpret_opcode(void) {
  /* figure out how to reduce the amount of shit in this? */
  static const instruction_pointer instructions[256] = {
      [0x69] = &ADC_im,
      [0x65] = &ADC_zero,
      [0x75] = &ADC_zerox,
      [0x6d] = &ADC_absolute,
      [0x7d] = &ADC_absolutex,
      [0x79] = &ADC_absolutey,
      [0x61] = &ADC_indirectx,
      [0x71] = &ADC_indirecty,

      [0x29] = &AND_im,
      [0x25] = &AND_zero,
      [0x35] = &AND_zerox,
      [0x2d] = &AND_absolute,
      [0x3d] = &AND_absolutex,
      [0x39] = &AND_absolutey,
      [0x21] = &AND_indirectx,
      [0x31] = &AND_indirecty,

      [0x0a] = &ASL_accumulator,
      [0x06] = &ASL_zero,
      [0x16] = &ASL_zerox,
      [0x0e] = &ASL_absolute,
      [0x1e] = &ASL_absolutex,

      [0x90] = &BCC,
      [0xb0] = &BCS,
      [0xf0] = &BEQ,

      [0x24] = &BIT_zero,
      [0x2c] = &BIT_absolute,

      [0x30] = &BMI,
      [0xd0] = &BNE,
      [0x10] = &BPL,
      [0x00] = &BRK,
      [0x50] = &BVC,
      [0x70] = &BVS,

      [0x18] = &CLC,
      [0xd8] = &CLD,
      [0x58] = &CLI,
      [0xb8] = &CLV,

      [0xc9] = &CMP_im,
      [0xc5] = &CMP_zero,
      [0xd5] = &CMP_zerox,
      [0xcd] = &CMP_absolute,
      [0xdd] = &CMP_absolutex,
      [0xd9] = &CMP_absolutey,
      [0xc1] = &CMP_indirectx,
      [0xd1] = &CMP_indirecty,

      [0xe0] = &CPX_im,
      [0xe4] = &CPX_zero,
      [0xec] = &CPX_absolute,
      [0xc0] = &CPY_im,
      [0xc4] = &CPY_zero,
      [0xcc] = &CPY_absolute,

      [0xc6] = &DEC_zero,
      [0xd6] = &DEC_zerox,
      [0xce] = &DEC_absolute,
      [0xde] = &DEC_absolutex,
      [0xca] = &DEX,
      [0x88] = &DEY,

      [0x49] = &EOR_im,
      [0x45] = &EOR_zero,
      [0x55] = &EOR_zerox,
      [0x4d] = &EOR_absolute,
      [0x5d] = &EOR_absolutex,
      [0x59] = &EOR_absolutey,
      [0x41] = &EOR_indirectx,
      [0x51] = &EOR_indirecty,

      [0xe6] = &INC_zero,
      [0xf6] = &INC_zerox,
      [0xee] = &INC_absolute,
      [0xfe] = &INC_absolutex,
      [0xe8] = &INX,
      [0xc8] = &INY,
      [0x4c] = &JMP_absolute,
      [0x6c] = &JMP_indirect,
      [0x20] = &JSR,

      [0xa9] = &LDA_im,
      [0xa5] = &LDA_zero,
      [0xb5] = &LDA_zerox,
      [0xad] = &LDA_absolute,
      [0xbd] = &LDA_absolutex,
      [0xb9] = &LDA_absolutey,
      [0xa1] = &LDA_indirectx,
      [0xb1] = &LDA_indirecty,

      [0xa2] = &LDX_im,
      [0xa6] = &LDX_zero,
      [0xb6] = &LDX_zeroy,
      [0xae] = &LDX_absolute,
      [0xbe] = &LDX_absolutey,
      [0xa0] = &LDY_im,
      [0xa4] = &LDY_zero,
      [0xb4] = &LDY_zerox,
      [0xac] = &LDY_absolute,
      [0xbc] = &LDY_absolutex,

      [0x4a] = &LSR_accumulator,
      [0x46] = &LSR_zero,
      [0x56] = &LSR_zerox,
      [0x4e] = &LSR_absolute,
      [0x5e] = &LSR_absolutex,

      [0x05] = &ORA_im,
      [0x09] = &ORA_zero,
      [0x15] = &ORA_zerox,
      [0x0d] = &ORA_absolute,
      [0x1d] = &ORA_absolutex,
      [0x19] = &ORA_absolutey,
      [0x01] = &ORA_indirectx,
      [0x11] = &ORA_indirecty,

      [0x48] = &PHA,
      [0x08] = &PHP,
      [0x68] = &PLA,
      [0x28] = &PLP,

      [0x2a] = &ROL_accumulator,
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
      [0xe9] = &SBC_im,
      [0xe5] = &SBC_zero,
      [0xf5] = &SBC_zerox,
      [0xed] = &SBC_absolute,
      [0xfd] = &SBC_absolutex,
      [0xf9] = &SBC_absolutey,
      [0xe1] = &SBC_indirectx,
      [0xf1] = &SBC_indirecty,

      [0x38] = &SEC,
      [0xf8] = &SED,
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
      [0x98] = &TYA,

      [0x0f] = &ASO_absolute,
      [0x1f] = &ASO_absolutex,
      [0x1b] = &ASO_absolutey,
      [0x07] = &ASO_zero,
      [0x17] = &ASO_zerox,
      [0x03] = &ASO_indirectx,
      [0x13] = &ASO_indirecty,
      [0x2f] = &RLA_absolute,
      [0x3f] = &RLA_absolutex,
      [0x3b] = &RLA_absolutey,
      [0x27] = &RLA_zero,
      [0x37] = &RLA_zerox,
      [0x23] = &RLA_indirectx,
      [0x33] = &RLA_indirecty,
      [0x4f] = &LSE_absolute,
      [0x5f] = &LSE_absolutex,
      [0x5b] = &LSE_absolutey,
      [0x47] = &LSE_zero,
      [0x57] = &LSE_zerox,
      [0x43] = &LSE_indirectx,
      [0x53] = &LSE_indirecty,
      [0x6f] = &RRA_absolute,
      [0x7f] = &RRA_absolutex,
      [0x7b] = &RRA_absolutey,
      [0x67] = &RRA_zero,
      [0x77] = &RRA_zerox,
      [0x63] = &RRA_indirectx,
      [0x73] = &RRA_indirecty,
      [0x8f] = &AXS_absolute,
      [0x87] = &AXS_zero,
      [0x97] = &AXS_zeroy,
      [0x83] = &AXS_indirectx,
      [0xaf] = &LAX_absolute,
      [0xa7] = &LAX_zero,
      [0xb7] = &LAX_zeroy,
      [0xa3] = &LAX_indirectx,
      [0xcf] = &DCM_absolute,
      [0xdf] = &DCM_absolutex,
      [0xdb] = &DCM_absolutey,
      [0xc7] = &DCM_zero,
      [0xd7] = &DCM_zerox,
      [0xc3] = &DCM_indirectx,
      [0xd3] = &DCM_indirecty,
      [0xef] = &INS_absolute,
      [0xff] = &INS_absolutex,
      [0xfb] = &INS_absolutey,
      [0xe7] = &INS_zero,
      [0xf7] = &INS_zerox,
      [0xe3] = &INS_indirectx,
      [0xf3] = &INS_indirecty,
      [0x9b] = &TAS,
      [0x9c] = &SAY,
      [0x9e] = &XAS,
      [0x9f] = &AXA_absolutey,
      [0x93] = &AXA_indirecty,

      [0xea] = &NOP,
      [0x80] = &NOP,
      [0x82] = &NOP,
      [0xc2] = &NOP,
      [0xe2] = &NOP,
      [0x04] = &NOP,
      [0x14] = &NOP,
      [0x34] = &NOP,
      [0x44] = &NOP,
      [0x54] = &NOP,
      [0x64] = &NOP,
      [0x74] = &NOP,
      [0xd4] = &NOP,
      [0xf4] = &NOP,

      [0x0c] = &SKW,
      [0x1c] = &SKW,
      [0x3c] = &SKW,
      [0x5c] = &SKW,
      [0x7c] = &SKW,
      [0x89] = &SKW,
      [0xdc] = &SKW,
      [0xfc] = &SKW,

      [0x02] = &HLT,
      [0x12] = &HLT,
      [0x22] = &HLT,
      [0x32] = &HLT,
      [0x42] = &HLT,
      [0x52] = &HLT,
      [0x62] = &HLT,
      [0x72] = &HLT,
      [0x92] = &HLT,
      [0xb2] = &HLT,
      [0xd2] = &HLT,
      [0xf2] = &HLT,
      [0x2b] = &ANC,
      [0x0b] = &ANC,
      [0xbb] = &LAS};

  unsigned char opcode = read_byte();

  if (instructions[opcode]) {
    instructions[opcode]();
  } else {
    printf("\033[1;31m invalid opcode: %x\033[0m\n", opcode);
    printf("PC: %x\n", registers->pc);
  }
}

static inline void push_to_stack(unsigned char value) {
  registers->stack_pointer[registers->_sp++] = value;
}

static inline unsigned char get_from_stack(void) {
  return registers->stack_pointer[--registers->_sp];
}

static inline unsigned char peek_from_stack(void) {
  return registers->stack_pointer[registers->_sp];
}

static inline uint8_t *indexed_indirect(uint8_t address) {
  uint16_t _location = read_word_at(address);
  uint16_t location = read_byte_at(_location + registers->x);
  return &memory[location];
}

static inline uint8_t *indirect_indexed(uint8_t address) {
  uint16_t _location = read_word_at(address);
  uint16_t location = read_byte_at(_location);
  return &memory[location + registers->y];
}

static inline uint8_t *dereference_address(uint8_t address) {
  return &memory[address];
}

static inline void set_flag(flags_t flag, bool b) {
  if (get_flag(flag) != b) {
    registers->status ^= flag;
  }
}

static inline uint8_t get_flag(flags_t flag) {
  /* !! to turn it into a 1 or 0 to not require shifting and
   * looking up the power of two it is, also this is faster than doing
   * status &= ~flag */
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
  /* algo to check if there's been an overflow, google for more info */
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

static void ADC_help(uint16_t value) {
  uint16_t result = registers->accumulator + value + get_flag(CARRY);
  overflow_check(value, result);
  carry_check(result);
  zero_check(result);
  negative_check(result);
  registers->accumulator = result;
}

static void ADC_im(void) {
  uint8_t value = read_byte();
  ADC_help(value);
}

static void ADC_zero(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location]);
}

static void ADC_zerox(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location + registers->x]);
}

static void ADC_zeroy(void) {
  uint8_t location = read_byte();
  ADC_help(memory[location + registers->y]);
}

static void ADC_absolute(void) {
  uint16_t location = read_word();
  ADC_help(memory[location]);
}

static void ADC_absolutex(void) {
  uint16_t location = read_word();
  ADC_help(memory[location + registers->x]);
}

static void ADC_absolutey(void) {
  uint16_t location = read_word();
  ADC_help(memory[location + registers->y]);
}

static void ADC_indirectx(void) {
  uint8_t location = read_byte();
  uint16_t value = read_word_at(location + registers->x);
  printf("%d\n", value);
  ADC_help(value);
}

static void ADC_indirecty(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_word_at(_location + registers->y);
  uint16_t value = read_word_at(location);

  ADC_help(value);
}

static void AND_help(uint8_t value) {
  registers->accumulator &= value;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}
static void AND_im(void) { AND_help(read_byte()); }

static void AND_zero(void) { AND_help(memory[read_byte()]); }

static void AND_zerox(void) { AND_help(memory[read_byte() + registers->x]); }

static void AND_absolute(void) { AND_help(memory[read_word()]); }

static void AND_absolutex(void) { AND_help(memory[read_word() + registers->x]); }

static void AND_absolutey(void) { AND_help(memory[read_word() + registers->y]); }

static void AND_indirectx(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location + registers->x);
  uint8_t value = read_byte_at(location);
  AND_help(value);
}

static void AND_indirecty(void) {
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

static void ASL_accumulator(void) { ASL_help(&registers->accumulator); }

static void ASL_zero(void) { ASL_help(&memory[read_byte()]); }

static void ASL_zerox(void) { ASL_help(&memory[read_byte() + registers->x]); }

static void ASL_absolute(void) { ASL_help(&memory[read_word()]); }

static void ASL_absolutex(void) { ASL_help(&memory[read_word() + registers->x]); }

static void BCC(void) {
  int8_t offset = read_byte();
  if (!get_flag(CARRY)) {
    registers->pc += offset;
  }
}

static void BCS(void) {
  int8_t offset = read_byte();
  if (get_flag(CARRY)) {
    registers->pc += offset;
  }
}

static void BEQ(void) {
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

static void BIT_zero(void) {
  uint8_t location = read_byte();
  uint8_t bit_test = registers->accumulator & memory[location];
  BIT_help(bit_test);
}

static void BIT_absolute(void) {
  uint16_t location = read_word();
  uint8_t bit_test = registers->accumulator & memory[location];
  BIT_help(bit_test);
}

static void BMI(void) {
  int8_t offset = read_byte();
  if (get_flag(NEGATIVE)) {
    registers->pc += offset;
  }
}

static void BNE(void) {
  int8_t offset = read_byte();
  if (!get_flag(ZERO)) {
    registers->pc += offset;
  }
}

static void BPL(void) {
  int8_t offset = read_byte();
  if (!get_flag(NEGATIVE)) {
    registers->pc += offset;
  }
}

static void BRK(void) {
  push_to_stack(registers->pc);
  push_to_stack(registers->status);
  set_flag(BREAK, true);
  registers->pc = 0xFFFE;
}

static void BVC(void) {
  int8_t offset = read_byte();
  if (!get_flag(OVERFLOW)) {
    registers->pc += offset;
  }
}
static void BVS(void) {
  int8_t offset = read_byte();
  if (get_flag(OVERFLOW)) {
    registers->pc += offset;
  }
}

static void CLC(void) { set_flag(CARRY, false); }

static void CLD(void) { set_flag(DECIMAL, false); }

static void CLI(void) { set_flag(INTERRUPT, false); }

static void CLV(void) { set_flag(OVERFLOW, false); }

/* this is for all compare instructions */
static void cmp_help(unsigned char value, unsigned char reg) {
  if (reg == value) {
    set_flag(ZERO, true);
  } else if (reg >= value) {
    set_flag(CARRY, true);
  }

  if ((signed char)(reg - value) < 0) {
    set_flag(NEGATIVE, true);
  }
}

static void CMP_help(unsigned char value) { cmp_help(value, registers->accumulator); }

static void CMP_im(void) {
  unsigned char value = read_byte();
  CMP_help(value);
}

static void CMP_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = memory[location];
  CMP_help(value);
}

static void CMP_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = memory[location + registers->x];
  CMP_help(value);
}

static void CMP_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = memory[location];
  CMP_help(value);
}

static void CMP_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = memory[location + registers->x];
  CMP_help(value);
}

static void CMP_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  CMP_help(value);
}

static void CMP_indirectx(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location + registers->x);
  uint8_t value = read_byte_at(location);
  CMP_help(value);
}

static void CMP_indirecty(void) {
  uint8_t _location = read_byte();
  uint16_t location = read_word_at(_location);
  uint8_t value = read_byte_at(location + registers->y);
  CMP_help(value);
}

void CPX_help(unsigned char value) { cmp_help(value, registers->x); }

static void CPX_im(void) {
  uint8_t value = read_byte();
  CPX_help(value);
}

static void CPX_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

static void CPX_absolute(void) {
  uint8_t location = read_word();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

static void CPY_help(unsigned char value) { cmp_help(value, registers->y); }

static void CPY_im(void) {
  uint8_t value = read_byte();
  CPX_help(value);
}

static void CPY_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

static void CPY_absolute(void) {
  uint8_t location = read_word();
  uint8_t value = read_byte_at(location);
  CPX_help(value);
}

void DEC_help(uint16_t location) {
  memory[location]--;
  zero_check(memory[location]);
  negative_check(memory[location]);
}

static void DEC_zero(void) {
  uint8_t location = read_byte();
  DEC_help(location);
}

static void DEC_zerox(void) {
  uint8_t location = read_byte();
  DEC_help(location);
}

static void DEC_absolute(void) {
  uint16_t location = read_word();
  DEC_help(location);
}

static void DEC_absolutex(void) {
  uint16_t location = read_word();
  DEC_help(location + registers->x);
}

static void DEX(void) {
  registers->x--;
  zero_check(registers->x);
  negative_check(registers->x);
}

static void DEY(void) {
  registers->y--;
  zero_check(registers->y);
  negative_check(registers->y);
}

static void EOR_help(uint8_t value) {
  registers->accumulator ^= value;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

static void EOR_im(void) {
  uint8_t value = read_byte();
  EOR_help(value);
}

static void EOR_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  EOR_help(value);
}

static void EOR_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  EOR_help(value);
}

static void EOR_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  EOR_help(value);
}

static void EOR_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->x);
  EOR_help(value);
}

static void EOR_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  EOR_help(value);
}

static void EOR_indirectx(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_byte_at(_location + registers->x);
  uint16_t value = read_word_at(location);
  EOR_help(value);
}

static void EOR_indirecty(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_byte_at(_location);
  uint16_t value = read_word_at(location);
  EOR_help(value);
}

static void INC_help(uint16_t location) {
  memory[location]++;
  zero_check(memory[location]);
  negative_check(memory[location]);
}

static void INC_zero(void) {
  uint8_t location = read_byte();
  INC_help(location);
}

static void INC_zerox(void) {
  uint8_t location = read_byte();
  INC_help(location);
}

static void INC_absolute(void) {
  uint8_t location = read_word();
  INC_help(location);
}

static void INC_absolutex(void) {
  uint8_t location = read_word();
  INC_help(location);
}

static void INX(void) {
  registers->x++;
  zero_check(registers->x);
  negative_check(registers->x);
}

static void INY(void) {
  registers->y++;
  zero_check(registers->y);
  negative_check(registers->y);
}

static void JMP_absolute(void) { registers->pc = read_word(); }

static void JMP_indirect(void) {
  uint16_t location = read_word();
  uint16_t jmp_to = read_word_at(location);
  registers->pc = jmp_to;
}

static void JSR(void) {
  registers->stack_pointer[registers->_sp] = registers->pc - 1;
  registers->pc = read_word();
}

void LDA_help(uint16_t value) {
  registers->accumulator = value;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

static void LDA_im(void) {
  uint8_t value = read_byte();
  printf("LDA value: %x\n", value);
  LDA_help(value);
}

static void LDA_zero(void) { LDA_help(memory[read_byte()]); }

static void LDA_zerox(void) { LDA_help(memory[read_byte() + registers->x]); }

static void LDA_absolute(void) { LDA_help(memory[read_word()]); }

static void LDA_absolutex(void) { LDA_help(memory[read_word() + registers->x]); }

static void LDA_absolutey(void) { LDA_help(memory[read_word() + registers->y]); }

static void LDA_indirectx(void) {
  uint8_t _location = read_byte() + registers->x;
  uint16_t location = read_word_at(_location);
  uint16_t value = read_word_at(location);
  printf("LDA indirect location: %x, value: %x\n", location, value);
  LDA_help(value);
}

static void LDA_indirecty(void) {
  uint8_t _location = read_byte() + registers->y;
  uint16_t location = read_word_at(_location);
  uint16_t value = read_word_at(location);
  printf("LDA value: %x\n", value);
  LDA_help(value);
}

static void LDX_help(uint8_t value) {
  registers->x = value;
  zero_check(registers->x);
  negative_check(registers->x);
}

static void LDX_im(void) {
  uint8_t value = read_byte();
  printf("ldx value: %x\n", value);
  LDX_help(value);
}

static void LDX_zero(void) { LDX_help(memory[read_byte()]); }

static void LDX_zeroy(void) { LDX_help(memory[read_byte() + registers->y]); }

static void LDX_absolute(void) { LDX_help(memory[read_word()]); }

static void LDX_absolutey(void) { LDX_help(memory[read_word() + registers->y]); }

static void LDY_help(uint8_t value) {
  registers->y = value;
  zero_check(registers->y);
  negative_check(registers->y);
}

static void LDY_im(void) { LDY_help(read_byte()); }

static void LDY_zero(void) { LDY_help(memory[read_byte()]); }

static void LDY_zerox(void) { LDY_help(memory[read_byte() + registers->x]); }

static void LDY_absolute(void) { LDY_help(memory[read_word()]); }

static void LDY_absolutex(void) { LDY_help(memory[read_word() + registers->y]); }

static void LSR_help(uint8_t *value) {
  *value >>= 1;
  carry_check(*value);
  zero_check(*value);
  negative_check(*value);
}

static void LSR_accumulator(void) { LSR_help(&registers->accumulator); }

static void LSR_zero(void) {
  uint8_t location = read_byte();
  LSR_help(dereference_address(location));
}

static void LSR_zerox(void) {
  uint8_t location = read_byte();
  LSR_help(dereference_address(location + registers->x));
}

static void LSR_absolute(void) {
  uint16_t location = read_word();
  LSR_help(dereference_address(location));
}

static void LSR_absolutex(void) {
  uint16_t location = read_word();
  LSR_help(dereference_address(location + registers->x));
}

static void NOP(void) { registers->pc++; }

static void ORA_help(uint16_t value) {
  registers->accumulator |= read_byte();
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

static void ORA_im(void) {
  uint8_t value = read_byte();
  ORA_help(value);
}

static void ORA_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

static void ORA_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

static void ORA_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

static void ORA_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->x);
  ORA_help(value);
}

static void ORA_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  ORA_help(value);
}

static void ORA_indirectx(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_word_at(_location + registers->x);
  uint8_t value = read_byte_at(location);
  ORA_help(value);
}

static void ORA_indirecty(void) {
  uint8_t _location = read_byte();
  uint8_t location = read_word_at(_location);
  uint8_t value = read_byte_at(location + registers->y);
  ORA_help(value);
}

static void PHA(void) { push_to_stack(read_byte()); }

static void PHP(void) { push_to_stack(registers->status); }

static void PLA(void) { registers->accumulator = get_from_stack(); }

static void PLP(void) { registers->status = get_from_stack(); }

void ROL_help(uint8_t *location) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  *location = (*location << 1) | (*location >> 7);
  carry_check(*location);
  zero_check(*location);
  negative_check(*location);
}

static void ROL_accumulator(void) { ROL_help(&registers->accumulator); }

static void ROL_zero(void) {
  uint8_t location = read_byte();
  ROL_help(dereference_address(location));
}

static void ROL_zerox(void) {
  uint8_t location = read_byte();
  ROL_help(dereference_address(location + registers->x));
}

static void ROL_absolute(void) {
  uint16_t location = read_word();
  ROL_help(dereference_address(location));
}

static void ROL_absolutex(void) {
  uint16_t location = read_word();
  ROL_help(dereference_address(location + registers->x));
}

void ROR_help(uint8_t *location) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  *location = (*location >> 1) | (*location << 7);
  carry_check(*location);
  zero_check(*location);
  negative_check(*location);
}

static void ROR_accumulator(void) { ROR_help(&registers->accumulator); }

static void ROR_zero(void) {
  uint8_t location = read_byte();
  ROR_help(dereference_address(location));
}

static void ROR_zerox(void) {
  uint8_t location = read_byte();
  ROR_help(dereference_address(location + registers->x));
}

static void ROR_absolute(void) {
  uint16_t location = read_word();
  ROR_help(dereference_address(location));
}

static void ROR_absolutex(void) {
  uint16_t location = read_word();
  ROR_help(dereference_address(location + registers->x));
}

static void RTI(void) {
  registers->status = get_from_stack();
  registers->pc++;
}

static void RTS(void) {
  registers->pc = get_from_stack();
  registers->pc++;
}

static void SBC_help(uint8_t amt) {
  uint16_t result = registers->accumulator - amt - get_flag(CARRY);
  overflow_check(amt, result);
  carry_check(result);
  zero_check(result);
  negative_check(result);
  registers->accumulator = result;
}

static void SBC_im(void) {
  uint8_t value = read_byte();
  SBC_help(value);
}

static void SBC_zero(void) {
  uint8_t location = read_byte();
  SBC_help(read_byte_at(location));
}

static void SBC_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  SBC_help(value);
}

static void SBC_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  SBC_help(value);
}

static void SBC_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->x);
  SBC_help(value);
}

static void SBC_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + registers->y);
  SBC_help(value);
}

static void SBC_indirectx(void) {
  uint16_t address = read_word();
  uint8_t *ptr = indexed_indirect(address);
  SBC_help(*ptr);
}

static void SBC_indirecty(void) {
  uint16_t address = read_word();
  uint8_t *ptr = indirect_indexed(address);
  SBC_help(*ptr);
}

static void SEC(void) { set_flag(CARRY, true); }
static void SED(void) { set_flag(DECIMAL, true); }
static void SEI(void) { set_flag(INTERRUPT, true); }

static void STA_zero(void) {
  uint8_t location = read_byte();
  write_byte(registers->accumulator, location);
}

static void STA_zerox(void) {
  uint8_t address = read_byte();
  write_byte(registers->accumulator, address + registers->x);
}

static void STA_absolute(void) {
  uint16_t address = read_word();
  write_byte(registers->accumulator, address);
}

static void STA_absolutex(void) {
  uint16_t address = read_word();
  write_byte(registers->accumulator, address + registers->x);
}

static void STA_absolutey(void) {
  uint16_t address = read_word();
  write_byte(registers->accumulator, address + registers->y);
}

static void STA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *ptr = indexed_indirect(address);
  *ptr = registers->accumulator;
}

static void STA_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t *ptr = indirect_indexed(address);
  *ptr = registers->accumulator;
}

static void STX_zero(void) { write_byte(registers->x, read_byte()); }
static void STX_zeroy(void) { write_byte(registers->x, read_byte()); }
static void STX_absolute(void) { write_byte(registers->x, read_word()); }

static void STY_zero(void) { write_byte(registers->y, read_byte()); }
static void STY_zerox(void) { write_byte(registers->y, read_byte() + registers->x); }
static void STY_absolute(void) {
  write_byte(registers->y, read_word() + registers->x);
}

static void TAX(void) {
  registers->x = registers->accumulator;
  zero_check(registers->x);
  negative_check(registers->x);
}

static void TAY(void) {
  registers->y = registers->accumulator;
  zero_check(registers->y);
  negative_check(registers->y);
}
static void TSX(void) {
  registers->x = peek_from_stack();
  zero_check(registers->x);
  negative_check(registers->x);
}

static void TXA(void) {
  registers->accumulator = peek_from_stack();
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}
static void TXS(void) {
  push_to_stack(registers->x);
  zero_check(registers->x);
  negative_check(registers->x);
}

static void TYA(void) {
  registers->accumulator = registers->y;
  zero_check(registers->accumulator);
  negative_check(registers->accumulator);
}

/* UNDOCUMENTED OPCODES */
/* SOURCE: http://www.ffd2.com/fridge/docs/6502-NMOS.extra.opcodes */

/* add status flags for the following ones, might be unnecessary tbh, because of
 * the fact they're done in the helper functions */
static void SKW(void) {
  read_word();
}

static void HLT(void) {
  exit(1);
}

static void ASO_absolute(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr= dereference_address(location);
  ASL_help(value_ptr);
  ORA_help(*value_ptr);
}

static void ASO_absolutex(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location + registers->x);
  ASL_help(value_ptr);
  ORA_help(*value_ptr);
}

static void ASO_absolutey(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location + registers->y);
  ASL_help(value_ptr);
  ORA_help(*value_ptr);
}

static void ASO_zero(void) {
  uint8_t location = read_byte();
  uint8_t *value = dereference_address(location);
  ASL_help(value);
  ORA_help(*value);
}

static void ASO_zerox(void) {
  uint8_t location = read_byte();
  uint8_t *value = &memory[location + registers->x];
  ASL_help(value);
  ORA_help(*value);
}

static void ASO_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  ASL_help(dereferenced);
  ORA_help(*dereferenced);
}

static void ASO_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indirect_indexed(address);
  ASL_help(dereferenced);
  ORA_help(*dereferenced);
}

static void RLA_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_absolutex(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->x);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_zero(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_zerox(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + registers->x);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indirect_indexed(address);
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void LSE_absolute(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location);
  LSR_help(value_ptr);
  EOR_help(*value_ptr);
}

static void LSE_absolutex(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->x);
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_zero(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_zerox(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indirect_indexed(address);
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void RRA_absolute(void){
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_absolutex(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->x);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_zero(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_zerox(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + registers->x);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_zeroy(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_indirectx(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_indirecty(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = indirect_indexed(address);
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void AXS_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  *dereferenced = registers->accumulator & registers->x;
}

static void AXS_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->x);
  *dereferenced = registers->accumulator & registers->x;
}

static void AXS_zero(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  *dereferenced = registers->accumulator & registers->x;
}

static void AXS_zeroy(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  *dereferenced = registers->accumulator & registers->x;
}

static void AXS_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  *dereferenced = registers->accumulator & registers->x;
}

static void AXS_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indirect_indexed(address);
  *dereferenced = registers->accumulator & registers->x;
}

static void LAX_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  registers->x = *dereferenced;
  registers->accumulator = *dereferenced;
}

static void LAX_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  registers->x = *dereferenced;
  registers->accumulator = *dereferenced;
}

static void LAX_zero(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  registers->x = *dereferenced;
  registers->accumulator = *dereferenced;
}

static void LAX_zeroy(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + registers->y);
  registers->x = *dereferenced;
  registers->accumulator = *dereferenced;
}

static void LAX_indirectx(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = indexed_indirect(address);
  registers->x = *dereferenced;
  registers->accumulator = *dereferenced;
}

static void LAX_indirecty(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = indexed_indirect(address);
  registers->x = *dereferenced;
  registers->accumulator = *dereferenced;
}

static void DCM_help(uint8_t *address) {
  (*address)--;
  cmp_help(*address, registers->accumulator);
}

static void DCM_absolute(void) {
  uint16_t address = read_word();
  DCM_help(dereference_address(address));
}

static void DCM_absolutex(void) {
  uint16_t address = read_word();
  DCM_help(dereference_address(address + registers->x));
}

static void DCM_absolutey(void) {
  uint16_t address = read_word();
  DCM_help(dereference_address(address + registers->y));
}

static void DCM_zero(void) {
  uint8_t address = read_byte();
  DCM_help(dereference_address(address));
}

static void DCM_zerox(void) {
  uint8_t address = read_byte();
  DCM_help(dereference_address(address + registers->x));
}

static void DCM_indirectx(void) {
  uint8_t address = read_byte();
  DCM_help(indexed_indirect(address));
}

static void DCM_indirecty(void) {
  uint8_t address = read_byte();
  DCM_help(indirect_indexed(address));
}

static void INS_help(uint8_t *address) {
  *address++;
  SBC_help(*address);
}

static void INS_absolute(void) {
	uint16_t address = read_word();
	INS_help(dereference_address(address));
}

static void INS_absolutex(void) {
	uint16_t address = read_word();
	INS_help(dereference_address(address + registers->x));
}

static void INS_absolutey(void) {
	uint16_t address = read_word();
	INS_help(dereference_address(address + registers->y));
}

static void INS_zero(void) {
	uint8_t address = read_byte();
  INS_help(dereference_address(address));
}

static void INS_zerox(void) {
	uint8_t address = read_byte();
  INS_help(dereference_address(address + registers->x));
}

static void INS_indirectx(void) {
	uint8_t address = read_byte();
  INS_help(indexed_indirect(address));
}

static void INS_indirecty(void) {
	uint8_t address = read_byte();
  INS_help(indirect_indexed(address));
}

static void ALR(void) {
  uint8_t value = read_byte();
  AND_help(value);
  LSR_help(&registers->accumulator);
}

static void TAS(void) {
  uint16_t location = read_word();
  uint8_t tmp = registers->accumulator & registers->x;

  push_to_stack(registers->x);
  tmp &= ((location >> 8) + 1);

  memory[location + registers->y] = tmp;
}

/* essentially the same as XAS but for Y register */
static void SAY(void) {
  uint16_t location = read_word();
  uint8_t value = registers->y & 0xf;
  memory[location + registers->x] = value;
}

static void XAS(void) {
  uint16_t location = read_word();
  uint8_t value = registers->x & 0xf;
  memory[location + registers->x] = value;
}

/* TODO */
static void AXA_absolutey(void) {
  uint16_t location = read_word();
  uint8_t written = registers->accumulator & registers->x & ((location >> 8) + 1);
  write_byte(written, location + registers->y);
}

static void AXA_indirecty(void) {
  uint8_t location = read_byte();
  uint8_t written = registers->accumulator & registers->x & (location + 1);
  write_byte(written, *indirect_indexed(location));
}

/* only has immediate */
static void ANC(void) {
  uint8_t value = read_byte();
  registers->accumulator &= value;
  set_flag(CARRY, !!(registers->accumulator & NEGATIVE));
}

static void LAS(void) {
  uint16_t location = read_word();
  registers->accumulator = read_byte_at(location) & peek_from_stack();

  negative_check(registers->accumulator);
  zero_check(registers->accumulator);
}
