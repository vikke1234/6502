#include "../headers/cpu.h"
#include "../headers/logger.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef __unix__
#include <unistd.h>

#elif defined(_WIN32) || defined(WIN32)
/* required to compile on windows */
#define __attribute__(x)
#define __always_inline__
#define inline __inline
#define access _access
#define F_OK 0
#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#endif

#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RED "\x1b[31m"
#define ANSI_END "\x1b[0m"

/* TODO:
 * add clock counter to make drawing easier
 */

typedef void (*instruction_pointer)(void);

/* for "easier" access to the different parts of memory for e.g. debugging
 * purpuses */
processor_t processor;

/* like 150 lines of prototypes, have fun :) */
static void ADC_absolute(void);
static void ADC_absolutex(void);
static void ADC_absolutey(void);
static void ADC_help(uint16_t value);
static void ADC_im(void);
static void ADC_indirectx(void);
static void ADC_indirecty(void);
static void ADC_zero(void);
static void ADC_zerox(void);
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
static void INC_absolute(void);
static void INC_absolutex(void);
static void INC_help(uint16_t location);
static void INC_zero(void);
static void INC_zerox(void);
static void INX(void);
static void INY(void);
static void JMP_absolute(void);
static void JMP_indirect(void);
static void JSR(void);
static void LDA_absolute(void);
static void LDA_absolutex(void);
static void LDA_absolutey(void);
static void LDA_help(uint8_t value);
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
static void LSR_absolute(void);
static void LSR_absolutex(void);
static void LSR_accumulator(void);
static void LSR_help(uint8_t *value);
static void LSR_zero(void);
static void LSR_zerox(void);
static void NOP_zero(void);
static void NOP_zerox(void);
static void NOP_im(void);
static void NOP_absolute(void);
static void NOP_absolutex(void);
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
static void RTI(void);
static void RTS(void);
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
static void TAX(void);
static void TAY(void);
static void TSX(void);
static void TXA(void);
static void TXS(void);
static void TYA(void);

#ifdef UNOFFICIAL_OPCODES

static void ALR(void);
static void ARR(void);
static void XAA(void);
static void ANC(void);
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
static void AXS_indirectx(void);
static void AXS_zero(void);
static void AXS_zeroy(void);
static void DCM_absolute(void);
static void DCM_absolutex(void);
static void DCM_absolutey(void);
static void DCM_help(uint8_t *address);
static void DCM_indirectx(void);
static void DCM_indirecty(void);
static void DCM_zero(void);
static void DCM_zerox(void);
static void HLT(void);
static void INS_absolute(void);
static void INS_absolutex(void);
static void INS_absolutey(void);
static void INS_help(uint8_t *address);
static void INS_indirectx(void);
static void INS_indirecty(void);
static void INS_zero(void);
static void INS_zerox(void);
static void LAS(void);
static void LAX_absolute(void);
static void LAX_absolutey(void);
static void LAX_indirectx(void);
static void LAX_indirecty(void);
static void LAX_zero(void);
static void LAX_zeroy(void);
static void LSE_absolute(void);
static void LSE_absolutex(void);
static void LSE_absolutey(void);
static void LSE_indirectx(void);
static void LSE_indirecty(void);
static void LSE_zero(void);
static void LSE_zerox(void);
static void OAL(void);
static void SAX(void);
static void RLA_absolute(void);
static void RLA_absolutex(void);
static void RLA_absolutey(void);
static void RLA_indirectx(void);
static void RLA_indirecty(void);
static void RLA_zero(void);
static void RLA_zerox(void);
static void RRA_absolute(void);
static void RRA_absolutex(void);
static void RRA_absolutey(void);
static void RRA_indirectx(void);
static void RRA_indirecty(void);
static void RRA_zero(void);
static void RRA_zerox(void);
static void SAY(void);
static void TAS(void);
static void XAS(void);

#endif
/**
 * @brief does comparison for the different CMP instructions
 */
static void cmp_help(unsigned char value, unsigned char reg);

/* theese just check for what the name says and sets the status based on that */
static inline void zero_check(uint8_t value);
static inline void carry_check(uint16_t value);
static inline void negative_check(uint8_t value);
static inline void overflow_check(uint16_t value, uint16_t result);

/**
 * @brief does a simple check if the page has been crossed
 *
 * @param address    The address to read from
 * @param reg        The register (x, y) to increment addr with
 * @return 1 if page crossed else 0
 */
static inline unsigned long long page_check(uint16_t address, uint8_t reg);

static inline uint8_t *dereference_address(uint16_t address);
static inline uint8_t *indexed_indirect(uint16_t address);
/* might get turnt into a macro so the function can access the address */

static inline unsigned char pop_from_stack(void);
static inline unsigned char peek_from_stack(void);
static inline void push_to_stack(unsigned char value);

static inline void write_byte(uint8_t value, uint16_t location);

/**
 * @brief reads 2 bytes (little endian) and combines them into
 a 16bit unsigned
 * assumes that thereemacs's atleast 2 bytes to read, note increment PC by 2
 */
static inline uint16_t read_word();

/**
 * @brief reads 1 byte (little endian), note increment PC
 */
static inline uint8_t read_byte();

/**
 * @brief same as @see{read_byte} but for a specific location
 */
static inline uint8_t read_byte_at(uint16_t location);

/**
 * @brief same as @see{read_word} but for a specific location
 */
static inline uint16_t read_word_at(uint16_t location);

extern void initialize_cpu(cartridge_t *cart) {
  if (cart == NULL) {
    fprintf(stderr, ANSI_RED "ERROR: cart is undefined" ANSI_END);
  }

  init_log();
  atexit(&close_log);

  processor.registers._sp = 0;
  processor.registers.accumulator = 0;
  processor.registers.x = 0;
  processor.registers.y = 0;
  processor.registers.pc = read_word_at(0xfffc);
  processor.registers.status = 0;

  memset(&processor.memory, 0xff, 0x07ff);
}

/**
   @brief given a file name, if it exists initialize the processor based on the
   data in it
   @param path to file
*/
extern bool initialize_cpu_filename(char *path) {
  if (path == NULL) {
    fprintf(stderr, ANSI_RED "ERROR: path NULL" ANSI_END);
    return false;
  }
  if (access(path, F_OK) == -1) {
    printf("\n\nerrno: %d\n\n", errno);
    switch (errno) {}
    return false;
  }
  struct stat buf;
  stat(path, &buf);
  // cartridge_t *cart = open_program(path);

  // initialize_cpu(cart);

  atexit(&close_log);
  init_log();

  FILE *fp = fopen(path, "rb");
  fread(processor.memory, 1, buf.st_size, fp);

  return true;
}

/**
   @brief for detection of infinite loops, currently set to 5
*/
static const int max_count = 5;
static int count = 0;
static int prev_pc = 0;

/**
   @brief a dirty hack
*/
static uint8_t current_opcode = 0;
static uint16_t current_pc = 0;
/* parser, pass data to initialize cpu and this does the rest */
extern void interpret_opcode(void) {
  /* there's a lot of boilerplate code, MAYBE it can be reduced with some macro
   * hax but probably not, plus it would be quite cryptic then (they do follow a
   * pattern) */
  static const instruction_pointer instructions[0x100] = {
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

      [0xea] = &NOP_im,
      [0x1a] = &NOP_im,
      [0x3a] = &NOP_im,
      [0x5a] = &NOP_im,
      [0x7a] = &NOP_im,
      [0xda] = &NOP_im,
      [0xfa] = &NOP_im,

      [0x80] = &NOP_im,
      [0x82] = &NOP_im,
      [0xc2] = &NOP_im,
      [0xe2] = &NOP_im,
      [0x04] = &NOP_zero,
      [0x14] = &NOP_zerox,
      [0x34] = &NOP_zerox,
      [0x44] = &NOP_zero,
      [0x54] = &NOP_zerox,
      [0x64] = &NOP_zero,
      [0x74] = &NOP_zerox,
      [0xd4] = &NOP_zerox,
      [0xf4] = &NOP_zerox,
      [0x0c] = &NOP_absolute,
      [0x1c] = &NOP_absolutex,
      [0x3c] = &NOP_absolutex,
      [0x5c] = &NOP_absolutex,
      [0x7c] = &NOP_absolutex,
      [0x89] = &NOP_absolutex,
      [0xdc] = &NOP_absolutex,
      [0xfc] = &NOP_absolutex,

#ifdef UNOFFICIAL_OPCODES
      [0x6b] = &ARR,
      [0x4b] = &ALR,
      [0x8b] = &XAA,

      [0x9f] = &AXA_absolutey,
      [0x93] = &AXA_indirecty,

      [0x0f] = &ASO_absolute,
      [0x1f] = &ASO_absolutex,
      [0x1b] = &ASO_absolutey,
      [0x07] = &ASO_zero,
      [0x17] = &ASO_zerox,
      [0x03] = &ASO_indirectx,
      [0x13] = &ASO_indirecty,

      [0xcb] = &SAX,
      [0xab] = &OAL,

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
      [0xbb] = &LAS,

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
      [0xbf] = &LAX_absolutey,
      [0xa7] = &LAX_zero,
      [0xb7] = &LAX_zeroy,
      [0xa3] = &LAX_indirectx,
      [0xb3] = &LAX_indirecty,

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
#endif
  };
  log_cpu(processor);
  unsigned char opcode = read_byte();

  if (processor.registers.pc == prev_pc) {
    if (count == max_count) {
      fprintf(stderr, "Infinite Loop\n");
      exit(1);
    }
    count++;
  }
  prev_pc = processor.registers.pc;

  if (instructions[opcode]) {
    current_opcode = opcode;
    current_pc = processor.registers.pc;
    instructions[opcode]();
  } else {
    printf("\033[1;31m invalid opcode: %x\033[0m\n", opcode);
    printf("PC: %x\n", processor.registers.pc);
  }
}

static inline void copy_to_stack(unsigned char value) {
  processor.memory[STACK_START + processor.registers._sp] = value;
}

static inline void push_to_stack(unsigned char value) {
  if (processor.registers._sp - 1 < processor.registers._sp) {
    processor.memory[STACK_START + (processor.registers._sp--)] = value;
  } else {
    fprintf(stderr, "Stack Overflow exception, exitting");
    exit(STACK_OVERFLOW);
  }
}

static inline unsigned char pop_from_stack(void) {
  if (processor.registers._sp + 1 > processor.registers._sp) {
    return processor.memory[STACK_START + (++processor.registers._sp)];
  } else {
    fprintf(stderr, "Stack Underflow exception, exitting");
    exit(STACK_UNDERFLOW);
  }
}

static inline unsigned char peek_from_stack(void) {
  return processor.memory[STACK_START + processor.registers._sp - 1];
}

static inline uint8_t *indexed_indirect(uint16_t address) {
  uint16_t _location = read_word_at(address);
  uint16_t location = read_byte_at(_location + processor.registers.x);
  return &processor.memory[location];
}

/* reasoning for being a macro is that for some undocumented opcodes
 * you need to check if it pages with `location` but not with all and it's
 * unnecessary to add some bool checks etc */
#define indirect_indexed(address)                                              \
  uint8_t _location = read_byte_at((address));                                 \
  uint8_t location = read_byte_at(_location);                                  \
  uint8_t *value = &processor.memory[location + processor.registers.y];

static inline __attribute__((__always_inline__)) uint8_t *
dereference_address(uint16_t address) {
  return &processor.memory[address]; /* always in range due to address being an
                              2 bytes long */
}

static inline uint16_t read_word() {
  uint16_t value = ((processor.memory[processor.registers.pc + 1] << 8) |
                    processor.memory[processor.registers.pc]);
  processor.registers.pc += 2;
  return value;
}

static inline uint16_t read_word_at(uint16_t location) {
  uint16_t value =
      (processor.memory[location + 1] << 8) | processor.memory[location];
  return value;
}

static inline uint8_t read_byte_at(uint16_t location) {
  uint8_t value = processor.memory[location];
  return value;
}

static inline uint8_t read_byte() {
  uint8_t value = processor.memory[processor.registers.pc++];
  return value;
}

static inline void write_byte(uint8_t value, uint16_t location) {
  processor.memory[location] = value;
}

extern registers_t dump_registers(void) { return processor.registers; }

static inline void zero_check(uint8_t value) {
  if (!value) {
    processor.registers._status.z = true;
  } else {
    processor.registers._status.z = false;
  }
}
static inline void negative_check(uint8_t value) {
  if (value & 0x80) {
    processor.registers._status.n = true;
  } else {
    processor.registers._status.n = false;
  }
}
static inline void overflow_check(uint16_t value, uint16_t result) {
  /* algo to check if there's been an overflow, google for more info */
  if ((result ^ processor.registers.accumulator) & (result ^ value) & 0x80) {
    processor.registers._status.v = true;
  } else {
    processor.registers._status.v = false;
  }
}

static inline unsigned long long page_check(uint16_t address, uint8_t reg) {
  if (((address + reg) & 0xff00) != (address & 0xff00)) {
    return 1llu; /* PAGED */
  }
  return 0llu;
}
static inline void carry_check(uint16_t value) {
  if (value & 0xff00) {
    processor.registers._status.c = true;
  } else {
    processor.registers._status.c = false;
  }
}

static void ADC_help(uint16_t value) {
  uint8_t result =
      processor.registers.accumulator + value + processor.registers._status.c;
  overflow_check(value, result);
  carry_check(result);
  zero_check(result);
  negative_check(result);
  processor.registers.accumulator = result;
}

static void ADC_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  ADC_help(value);
}

static void ADC_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 3;
  ADC_help(processor.memory[location]);
}

static void ADC_zerox(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 4;
  ADC_help(processor.memory[location + processor.registers.x]);
}

static void ADC_absolute(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 4;
  ADC_help(processor.memory[location]);
}

static void ADC_absolutex(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 3 + page_check(location, processor.registers.x);
  ADC_help(processor.memory[location + processor.registers.x]);
}

static void ADC_absolutey(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 3 + page_check(location, processor.registers.y);
  ADC_help(processor.memory[location + processor.registers.y]);
}

static void ADC_indirectx(void) {
  uint8_t location = read_byte();
  uint16_t value = read_word_at(location + processor.registers.x);
  processor.clock_ticks += 6;
  ADC_help(value);
}

static void ADC_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  ADC_help(*value);
}

static void AND_help(uint8_t value) {
  processor.registers.accumulator &= value;
  zero_check(processor.registers.accumulator);
  negative_check(processor.registers.accumulator);
}
static void AND_im(void) {
  processor.clock_ticks += 2;
  uint8_t value = read_byte();
  AND_help(value);
}

static void AND_zero(void) {
  processor.clock_ticks += 3;
  uint8_t value = processor.memory[read_byte()];
  AND_help(value);
}

static void AND_zerox(void) {
  processor.clock_ticks += 4;
  uint8_t location = read_byte();
  AND_help(processor.memory[location + processor.registers.x]);
}

static void AND_absolute(void) {
  processor.clock_ticks += 4;
  uint16_t location = read_word();
  AND_help(processor.memory[location]);
}

static void AND_absolutex(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.x);

  AND_help(processor.memory[address + processor.registers.x]);
}

static void AND_absolutey(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.x);
  AND_help(processor.memory[address + processor.registers.y]);
}

static void AND_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *value = indexed_indirect(address);
  processor.clock_ticks += 6;
  AND_help(*value);
}

static void AND_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  AND_help(*value);
}

void ASL_help(uint8_t *address) {
  *address <<= 1;
  zero_check(*address);
  carry_check(*address);
  negative_check(*address);
}

static void ASL_accumulator(void) {
  processor.clock_ticks += 2;
  ASL_help(&processor.registers.accumulator);
}

static void ASL_zero(void) {
  processor.clock_ticks += 5;
  uint8_t location = read_byte();
  ASL_help(&processor.memory[location]);
}

static void ASL_zerox(void) {
  processor.clock_ticks += 6;
  uint8_t location = read_byte();
  ASL_help(&processor.memory[location + processor.registers.x]);
}

static void ASL_absolute(void) {
  processor.clock_ticks += 6;
  uint16_t location = read_word();
  ASL_help(&processor.memory[location]);
}

static void ASL_absolutex(void) {
  processor.clock_ticks += 7;
  uint16_t location = read_word();
  ASL_help(&processor.memory[location + processor.registers.x]);
}

static void branch(bool flag, bool b) {
  int8_t offset = read_byte();
  processor.clock_ticks += 2;
  if (flag == b) {
    processor.clock_ticks += 1 + page_check(processor.registers.pc, offset);
    processor.registers.pc += offset;
  }
}
static void BCC(void) { branch(processor.registers._status.c, false); }

static void BCS(void) { branch(processor.registers._status.c, true); }

static void BEQ(void) { branch(processor.registers._status.z, true); }

void BIT_help(uint16_t bit_test) {
  if (bit_test == 0) {
    processor.registers._status.z = true;
    processor.registers._status.v = false;
    processor.registers._status.n = false;
  } else {
    processor.registers._status.v = !!(bit_test & OVERFLOW);
    processor.registers._status.n = !!(bit_test & NEGATIVE);
  }
}

static void BIT_zero(void) {
  uint8_t location = read_byte();
  uint8_t bit_test =
      processor.registers.accumulator & processor.memory[location];
  processor.clock_ticks += 3;
  BIT_help(bit_test);
}

static void BIT_absolute(void) {
  uint16_t location = read_word();
  uint8_t bit_test =
      processor.registers.accumulator & processor.memory[location];
  processor.clock_ticks += 4;
  BIT_help(bit_test);
}

static void BMI(void) { branch(processor.registers._status.n, true); }

static void BNE(void) { branch(processor.registers._status.z, false); }

static void BPL(void) { branch(processor.registers._status.n, false); }

/* TODO */
static void BRK(void) {
  processor.registers.pc++;
  push_to_stack(processor.registers.pc & 0xff);
  push_to_stack((processor.registers.pc & 0xff00) >> 8);
  push_to_stack(processor.registers.status);

  processor.registers._status.b = true;
  processor.registers.pc = read_word_at(0xfffe);
  processor.clock_ticks += 7;
}

static void BVC(void) { branch(processor.registers._status.v, false); }

static void BVS(void) { branch(processor.registers._status.v, true); }

static void CLC(void) {
  processor.clock_ticks += 2;
  processor.registers._status.c = false;
}

static void CLD(void) {
  processor.clock_ticks += 2;
  processor.registers._status.d = false;
}

static void CLI(void) {
  processor.clock_ticks += 2;
  processor.registers._status.i = false;
}

static void CLV(void) {
  processor.clock_ticks += 2;
  processor.registers._status.v = false;
}

/* this is for all compare instructions */
static void cmp_help(unsigned char value, unsigned char reg) {
  processor.registers._status.z = reg == value;
  processor.registers._status.c = reg >= value;
  processor.registers._status.n = (signed char)(reg - value) < 0;
}

/* this is simply to have less args for the caller as it's pointless to write
 * out accumulator each time */
static void CMP_help(unsigned char value) {
  cmp_help(value, processor.registers.accumulator);
}

static void CMP_im(void) {
  unsigned char value = read_byte();
  processor.clock_ticks += 2;
  CMP_help(value);
}

static void CMP_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = processor.memory[location];
  processor.clock_ticks += 3;
  CMP_help(value);
}

static void CMP_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = processor.memory[location + processor.registers.x];
  processor.clock_ticks += 4;
  CMP_help(value);
}

static void CMP_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = processor.memory[location];
  processor.clock_ticks += 4;
  CMP_help(value);
}

static void CMP_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = processor.memory[location + processor.registers.x];
  processor.clock_ticks += 4 + page_check(location, processor.registers.x);
  CMP_help(value);
}

static void CMP_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.y);
  processor.clock_ticks += 4 + page_check(location, processor.registers.y);
  CMP_help(value);
}

static void CMP_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *value = indexed_indirect(address);
  processor.clock_ticks += 6;
  CMP_help(*value);
}

static void CMP_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  CMP_help(*value);
}

static void CPX_help(unsigned char value) {
  cmp_help(value, processor.registers.x);
}

static void CPX_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  CPX_help(value);
}

static void CPX_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 3;
  CPX_help(value);
}

static void CPX_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  CPX_help(value);
}

static void CPY_help(unsigned char value) {
  cmp_help(value, processor.registers.y);
}

static void CPY_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  CPY_help(value);
}

static void CPY_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 3;
  CPY_help(value);
}

static void CPY_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  CPY_help(value);
}

static void DEC_help(uint16_t location) {
  processor.memory[location]--;
  zero_check(processor.memory[location]);
  negative_check(processor.memory[location]);
}

static void DEC_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 5;
  DEC_help(location);
}

static void DEC_zerox(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 6;
  DEC_help(location);
}

static void DEC_absolute(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 6;
  DEC_help(location);
}

static void DEC_absolutex(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 7;
  DEC_help(location + processor.registers.x);
}

static void DEX(void) {
  processor.registers.x--;
  zero_check(processor.registers.x);
  negative_check(processor.registers.x);
  processor.clock_ticks += 2;
}

static void DEY(void) {
  processor.registers.y--;
  zero_check(processor.registers.y);
  negative_check(processor.registers.y);
  processor.clock_ticks += 2;
}

static void EOR_help(uint8_t value) {
  processor.registers.accumulator ^= value;
  zero_check(processor.registers.accumulator);
  negative_check(processor.registers.accumulator);
}

static void EOR_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  EOR_help(value);
}

static void EOR_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 3;
  EOR_help(value);
}

static void EOR_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  EOR_help(value);
}

static void EOR_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  EOR_help(value);
}

static void EOR_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.x);
  processor.clock_ticks += 4 + page_check(location, processor.registers.x);
  EOR_help(value);
}

static void EOR_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.y);
  processor.clock_ticks += 4 + page_check(location, processor.registers.y);
  EOR_help(value);
}

static void EOR_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *value = indexed_indirect(address);
  processor.clock_ticks += 6;
  EOR_help(*value);
}

static void EOR_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  EOR_help(*value);
}

static void INC_help(uint16_t location) {
  processor.memory[location]++;
  zero_check(processor.memory[location]);
  negative_check(processor.memory[location]);
}

static void INC_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 5;
  INC_help(location);
}

static void INC_zerox(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 6;
  INC_help(location);
}

static void INC_absolute(void) {
  uint8_t location = read_word();
  processor.clock_ticks += 6;
  INC_help(location);
}

static void INC_absolutex(void) {
  uint8_t location = read_word();
  processor.clock_ticks += 7;
  INC_help(location);
}

static void INX(void) {
  processor.registers.x++;
  processor.clock_ticks += 2;
  zero_check(processor.registers.x);
  negative_check(processor.registers.x);
}

static void INY(void) {
  processor.registers.y++;
  processor.clock_ticks += 2;
  zero_check(processor.registers.y);
  negative_check(processor.registers.y);
}

static void JMP_absolute(void) {
  processor.clock_ticks += 3;
  uint16_t location = read_word();
  processor.registers.pc = location;
}

static void JMP_indirect(void) {
  uint16_t location = read_word();
  uint16_t jmp_to = read_word_at(location);
  processor.clock_ticks += 5;
  processor.registers.pc = jmp_to;
}

static void JSR(void) {
  processor.memory[STACK_START + processor.registers._sp] =
      processor.registers.pc - 1;
  uint16_t location = read_word();
  processor.registers.pc = location;
  processor.clock_ticks += 6;
}

void LDA_help(uint8_t value) {
  processor.registers.accumulator = value;
  zero_check(processor.registers.accumulator);
  negative_check(processor.registers.accumulator);
}

static void LDA_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  LDA_help(value);
}

static void LDA_zero(void) {
  processor.clock_ticks += 3;
  uint16_t location = read_byte();
  LDA_help(processor.memory[location]);
}

static void LDA_zerox(void) {
  processor.clock_ticks += 4;
  uint8_t location = read_byte();
  LDA_help(processor.memory[location + processor.registers.x]);
}

static void LDA_absolute(void) {
  processor.clock_ticks += 4;
  uint16_t location = read_word();
  LDA_help(processor.memory[location]);
}

static void LDA_absolutex(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.x);
  LDA_help(processor.memory[address + processor.registers.x]);
}

static void LDA_absolutey(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.y);
  LDA_help(processor.memory[address + processor.registers.y]);
}

static void LDA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *value = indexed_indirect(address);
  processor.clock_ticks += 6;
  LDA_help(*value);
}

static void LDA_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  LDA_help(*value);
}

static void LDX_help(uint8_t value) {
  processor.registers.x = value;
  zero_check(processor.registers.x);
  negative_check(processor.registers.x);
}

static void LDX_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  LDX_help(value);
}

static void LDX_zero(void) {
  processor.clock_ticks += 3;
  LDX_help(processor.memory[read_byte()]);
}

static void LDX_zeroy(void) {
  processor.clock_ticks += 4;
  LDX_help(processor.memory[read_byte() + processor.registers.y]);
}

static void LDX_absolute(void) {
  processor.clock_ticks += 4;
  LDX_help(processor.memory[read_word()]);
}

static void LDX_absolutey(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.y);
  LDX_help(processor.memory[address + processor.registers.y]);
}

static void LDY_help(uint8_t value) {
  processor.registers.y = value;
  zero_check(processor.registers.y);
  negative_check(processor.registers.y);
}

static void LDY_im(void) {
  processor.clock_ticks += 2;
  LDY_help(read_byte());
}

static void LDY_zero(void) {
  processor.clock_ticks += 3;
  LDY_help(processor.memory[read_byte()]);
}

static void LDY_zerox(void) {
  processor.clock_ticks += 4;
  LDY_help(processor.memory[read_byte() + processor.registers.x]);
}

static void LDY_absolute(void) {
  processor.clock_ticks += 4;
  LDY_help(processor.memory[read_word()]);
}

static void LDY_absolutex(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.x);
  LDY_help(processor.memory[read_word() + processor.registers.x]);
}

static void LSR_help(uint8_t *value) {
  (*value) >>= 1;
  carry_check(*value);
  zero_check(*value);
  negative_check(*value);
}

static void LSR_accumulator(void) {
  processor.clock_ticks += 2;
  LSR_help(&processor.registers.accumulator);
}

static void LSR_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 5;
  LSR_help(dereference_address(location));
}

static void LSR_zerox(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 6;
  LSR_help(dereference_address(location + processor.registers.x));
}

static void LSR_absolute(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 6;
  LSR_help(dereference_address(location));
}

static void LSR_absolutex(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 7;
  LSR_help(dereference_address(location + processor.registers.x));
}

static void NOP_im(void) { processor.clock_ticks += 2; }

static void NOP_absolute(void) {
  processor.clock_ticks += 4;
  processor.registers.pc += 2;
}

static void NOP_absolutex(void) {
  uint8_t address = read_word();
  processor.clock_ticks += 4 + page_check(address, processor.registers.x);
}
static void NOP_zero(void) { processor.clock_ticks += 3; }

static void NOP_zerox(void) { processor.clock_ticks += 4; }

static void ORA_help(uint16_t value) {
  processor.registers.accumulator |= value;
  zero_check(processor.registers.accumulator);
  negative_check(processor.registers.accumulator);
}

static void ORA_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  ORA_help(value);
}

static void ORA_zero(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 3;
  ORA_help(value);
}

static void ORA_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  ORA_help(value);
}

static void ORA_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  ORA_help(value);
}

static void ORA_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.x);
  processor.clock_ticks += 4 + page_check(location, processor.registers.x);
  ORA_help(value);
}

static void ORA_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.y);
  processor.clock_ticks += 4 + page_check(location, processor.registers.y);
  ORA_help(value);
}

static void ORA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *value = indexed_indirect(address);
  processor.clock_ticks += 6;
  ORA_help(*value);
}

static void ORA_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  ORA_help(*value);
}

static void PHA(void) {
  push_to_stack(processor.registers.accumulator);
  processor.clock_ticks += 3;
}

static void PHP(void) {
  processor.clock_ticks += 3;
  push_to_stack(processor.registers.status);
}

static void PLA(void) {
  processor.clock_ticks += 4;
  processor.registers.accumulator = pop_from_stack();
}

static void PLP(void) {
  processor.clock_ticks += 4;
  processor.registers.status = pop_from_stack();
}

static void ROL_help(uint8_t *location) {
  *location = (*location << 1) | (*location >> 7);
  carry_check(*location);
  zero_check(*location);
  negative_check(*location);
}

static void ROL_accumulator(void) {
  processor.clock_ticks += 2;
  ROL_help(&processor.registers.accumulator);
}

static void ROL_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 5;
  ROL_help(dereference_address(location));
}

static void ROL_zerox(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 6;
  ROL_help(dereference_address(location + processor.registers.x));
}

static void ROL_absolute(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 6;
  ROL_help(dereference_address(location));
}

static void ROL_absolutex(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 7;
  ROL_help(dereference_address(location + processor.registers.x));
}

static void ROR_help(uint8_t *location) {
  /* shifts the accumulator (to be changed later to support different addressing
   * modes) by 1 */
  *location = (*location >> 1) | (*location << 7);
  carry_check(*location);
  zero_check(*location);
  negative_check(*location);
}

static void ROR_accumulator(void) {
  processor.clock_ticks += 2;
  ROR_help(&processor.registers.accumulator);
}

static void ROR_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 5;
  ROR_help(dereference_address(location));
}

static void ROR_zerox(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 6;
  ROR_help(dereference_address(location + processor.registers.x));
}

static void ROR_absolute(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 6;
  ROR_help(dereference_address(location));
}

static void ROR_absolutex(void) {
  uint16_t location = read_word();
  processor.clock_ticks += 7;
  ROR_help(dereference_address(location + processor.registers.x));
}

static void RTI(void) {
  processor.registers.status = pop_from_stack();
  processor.registers.pc++;
  processor.clock_ticks += 6;
}

static void RTS(void) {
  processor.registers.pc = pop_from_stack();
  processor.registers.pc++;
  processor.clock_ticks += 6;
}

static void SBC_help(uint8_t amt) {
  uint8_t result =
      processor.registers.accumulator - amt - processor.registers._status.c;
  overflow_check(amt, result);
  carry_check(result);
  zero_check(result);
  negative_check(result);
  processor.registers.accumulator = result;
}

static void SBC_im(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  SBC_help(value);
}

static void SBC_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 3;
  SBC_help(read_byte_at(location));
}

static void SBC_zerox(void) {
  uint8_t location = read_byte();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  SBC_help(value);
}

static void SBC_absolute(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location);
  processor.clock_ticks += 4;
  SBC_help(value);
}

static void SBC_absolutex(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.x);
  processor.clock_ticks += 4 + page_check(location, processor.registers.x);
  SBC_help(value);
}

static void SBC_absolutey(void) {
  uint16_t location = read_word();
  uint8_t value = read_byte_at(location + processor.registers.y);
  processor.clock_ticks += 4 + page_check(location, processor.registers.y);
  SBC_help(value);
}

static void SBC_indirectx(void) {
  uint16_t address = read_word();
  uint8_t *ptr = indexed_indirect(address);
  processor.clock_ticks += 6;
  SBC_help(*ptr);
}

static void SBC_indirecty(void) {
  uint16_t address = read_word();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  SBC_help(*value);
}

static void SEC(void) {
  processor.clock_ticks += 2;
  processor.registers._status.c = true;
}
static void SED(void) {
  processor.clock_ticks += 2;
  processor.registers._status.d = true;
}
static void SEI(void) {
  processor.clock_ticks += 2;
  processor.registers._status.i = true;
}

static void STA_zero(void) {
  uint8_t location = read_byte();
  processor.clock_ticks += 3;
  write_byte(processor.registers.accumulator, location);
}

static void STA_zerox(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 4;
  write_byte(processor.registers.accumulator, address + processor.registers.x);
}

static void STA_absolute(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4;
  write_byte(processor.registers.accumulator, address);
}

static void STA_absolutex(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4;
  write_byte(processor.registers.accumulator, address + processor.registers.x);
}

static void STA_absolutey(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 4;
  write_byte(processor.registers.accumulator, address + processor.registers.y);
}

static void STA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *ptr = indexed_indirect(address);
  processor.clock_ticks += 6;
  *ptr = processor.registers.accumulator;
}

static void STA_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 6;
  *value = processor.registers.accumulator;
}

static void STX_zero(void) {
  processor.clock_ticks += 3;
  uint8_t location = read_byte();
  write_byte(processor.registers.x, location);
}
static void STX_zeroy(void) {
  processor.clock_ticks += 4;
  uint8_t location = read_byte();
  write_byte(processor.registers.x, location);
}
static void STX_absolute(void) {
  processor.clock_ticks += 4;
  uint16_t location = read_word();
  write_byte(processor.registers.x, location);
}

static void STY_zero(void) {
  uint8_t location = read_byte();
  write_byte(processor.registers.y, location);
  processor.clock_ticks += 3;
}
static void STY_zerox(void) {
  uint8_t location = read_byte();
  write_byte(processor.registers.y, location + processor.registers.x);
  processor.clock_ticks += 4;
}
static void STY_absolute(void) {
  uint16_t location = read_word();
  write_byte(processor.registers.y, location + processor.registers.x);
  processor.clock_ticks += 4;
}

static void TAX(void) {
  processor.registers.x = processor.registers.accumulator;
  zero_check(processor.registers.x);
  negative_check(processor.registers.x);
  processor.clock_ticks += 2;
}

static void TAY(void) {
  processor.registers.y = processor.registers.accumulator;
  zero_check(processor.registers.y);
  negative_check(processor.registers.y);
  processor.clock_ticks += 4;
}
static void TSX(void) {
  processor.registers.x = processor.registers._sp;
  zero_check(processor.registers.x);
  negative_check(processor.registers.x);
  processor.clock_ticks += 2;
}

static void TXA(void) {
  processor.registers.accumulator = processor.registers.x;
  zero_check(processor.registers.accumulator);
  negative_check(processor.registers.accumulator);
  processor.clock_ticks += 2;
}

static void TXS(void) {
  copy_to_stack(processor.registers.x);
  zero_check(processor.registers.x);
  negative_check(processor.registers.x);
  processor.clock_ticks += 2;
}

static void TYA(void) {
  processor.registers.accumulator = processor.registers.y;
  zero_check(processor.registers.accumulator);
  negative_check(processor.registers.accumulator);
  processor.clock_ticks += 2;
}

/* UNDOCUMENTED OPCODES */
/* SOURCE: http://www.ffd2.com/fridge/docs/6502-NMOS.extra.opcodes */

/* add status flags for the following ones, might be unnecessary tbh, because of
 * the fact they're done in the helper functions */
#ifdef UNOFFICIAL_OPCODES
static void HLT(void) { exit(1); }

static void ASO_absolute(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location);
  processor.clock_ticks += 6;
  ASL_help(value_ptr);
  ORA_help(*value_ptr);
}

static void ASO_absolutex(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location + processor.registers.x);
  processor.clock_ticks += 7;
  ASL_help(value_ptr);
  ORA_help(*value_ptr);
}

static void ASO_absolutey(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location + processor.registers.y);
  processor.clock_ticks += 7;
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
  uint8_t *value = &processor.memory[location + processor.registers.x];
  processor.clock_ticks += 5;
  ASL_help(value);
  ORA_help(*value);
}

static void ASO_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  processor.clock_ticks += 8;
  ASL_help(dereferenced);
  ORA_help(*dereferenced);
}

static void ASO_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 8;
  ASL_help(value);
  ORA_help(*value);
}

static void RLA_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 6;
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_absolutex(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.x);
  processor.clock_ticks += 7;
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.y);
  processor.clock_ticks += 7;
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_zero(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 5;
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_zerox(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + processor.registers.x);
  processor.clock_ticks += 6;
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  processor.clock_ticks += 8;
  ROL_help(dereferenced);
  AND_help(*dereferenced);
}

static void RLA_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 8;
  ROL_help(value);
  AND_help(*value);
}

static void LSE_absolute(void) {
  uint16_t location = read_word();
  uint8_t *value_ptr = dereference_address(location);
  processor.clock_ticks += 6;
  LSR_help(value_ptr);
  EOR_help(*value_ptr);
}

static void LSE_absolutex(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.x);
  processor.clock_ticks += 7;
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.y);
  processor.clock_ticks += 7;
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_zero(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 5;
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_zerox(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 6;
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  processor.clock_ticks += 8;
  LSR_help(dereferenced);
  EOR_help(*dereferenced);
}

static void LSE_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 8;
  LSR_help(value);
  EOR_help(*value);
}

static void RRA_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 6;
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_absolutex(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.x);
  processor.clock_ticks += 7;
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.y);
  processor.clock_ticks += 7;
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_zero(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 5;
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_zerox(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + processor.registers.x);
  processor.clock_ticks += 6;
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_indirectx(void) {
  uint16_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  processor.clock_ticks += 8;
  ROR_help(dereferenced);
  ADC_help(*dereferenced);
}

static void RRA_indirecty(void) {
  uint16_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 8;
  ROR_help(value);
  ADC_help(*value);
}

static void AXS_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 4;
  *dereferenced = processor.registers.accumulator & processor.registers.x;
}

static void AXS_zero(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 3;
  *dereferenced = processor.registers.accumulator & processor.registers.x;
}

static void AXS_zeroy(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = dereference_address(address + processor.registers.y);
  processor.clock_ticks += 4;
  *dereferenced = processor.registers.accumulator & processor.registers.x;
}

static void AXS_indirectx(void) {
  uint8_t address = read_byte();
  uint8_t *dereferenced = indexed_indirect(address);
  processor.clock_ticks += 6;
  *dereferenced = processor.registers.accumulator & processor.registers.x;
}

static void LAX_absolute(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 4;
  processor.registers.x = *dereferenced;
  processor.registers.accumulator = *dereferenced;
}

static void LAX_absolutey(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.y);
  processor.clock_ticks += 4 + page_check(address, processor.registers.y);
  processor.registers.x = *dereferenced;
  processor.registers.accumulator = *dereferenced;
}

static void LAX_zero(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address);
  processor.clock_ticks += 3;
  processor.registers.x = *dereferenced;
  processor.registers.accumulator = *dereferenced;
}

static void LAX_zeroy(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = dereference_address(address + processor.registers.y);
  processor.clock_ticks += 4;
  processor.registers.x = *dereferenced;
  processor.registers.accumulator = *dereferenced;
}

static void LAX_indirectx(void) {
  uint16_t address = read_word();
  uint8_t *dereferenced = indexed_indirect(address);
  processor.clock_ticks += 6;
  processor.registers.x = *dereferenced;
  processor.registers.accumulator = *dereferenced;
}

static void LAX_indirecty(void) {
  uint16_t address = read_word();
  indirect_indexed(address);
  processor.clock_ticks += 5 + page_check(location, processor.registers.y);
  processor.registers.x = *value;
  processor.registers.accumulator = *value;
}

static void DCM_help(uint8_t *address) {
  (*address)--;
  cmp_help(*address, processor.registers.accumulator);
}

static void DCM_absolute(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 6;
  DCM_help(dereference_address(address));
}

static void DCM_absolutex(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 7;
  DCM_help(dereference_address(address + processor.registers.x));
}

static void DCM_absolutey(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 7;
  DCM_help(dereference_address(address + processor.registers.y));
}

static void DCM_zero(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 5;
  DCM_help(dereference_address(address));
}

static void DCM_zerox(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 6;
  DCM_help(dereference_address(address + processor.registers.x));
}

static void DCM_indirectx(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 8;
  DCM_help(indexed_indirect(address));
}

static void DCM_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 8;
  DCM_help(value);
}

static void INS_help(uint8_t *address) {
  (*address)++;
  SBC_help(*address);
}

static void INS_absolute(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 6;
  INS_help(dereference_address(address));
}

static void INS_absolutex(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 7;
  INS_help(dereference_address(address + processor.registers.x));
}

static void INS_absolutey(void) {
  uint16_t address = read_word();
  processor.clock_ticks += 7;
  INS_help(dereference_address(address + processor.registers.y));
}

static void INS_zero(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 5;
  INS_help(dereference_address(address));
}

static void INS_zerox(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 6;
  INS_help(dereference_address(address + processor.registers.x));
}

static void INS_indirectx(void) {
  uint8_t address = read_byte();
  processor.clock_ticks += 7;
  INS_help(indexed_indirect(address));
}

static void INS_indirecty(void) {
  uint8_t address = read_byte();
  indirect_indexed(address);
  processor.clock_ticks += 8;
  INS_help(value);
}

static void ALR(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  AND_help(value);
  LSR_help(&processor.registers.accumulator);
}

static void ARR(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  processor.registers.accumulator &= value;
  ROR_help(&processor.registers.accumulator);
}

static void XAA(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  processor.registers.accumulator = processor.registers.x & value;
}

static void OAL(void) {
  uint8_t value = read_byte();
  processor.clock_ticks += 2;
  ORA_help(0xee);
  AND_help(value);
  processor.registers.x = processor.registers.accumulator;
}

static void SAX(void) {
  uint8_t value = read_byte();
  uint8_t calc_accumulator = processor.registers.accumulator;
  processor.clock_ticks += 2;
  calc_accumulator &= processor.registers.x;
  calc_accumulator -= value;
  carry_check(calc_accumulator);
  processor.registers.x = calc_accumulator;
}
static void TAS(void) {
  uint16_t location = read_word();
  uint8_t tmp = processor.registers.accumulator & processor.registers.x;
  processor.clock_ticks += 5;
  push_to_stack(processor.registers.x);
  tmp &= ((location >> 8) + 1);

  processor.memory[location + processor.registers.y] = tmp;
}

/* essentially the same as XAS but for Y register */
static void SAY(void) {
  uint16_t location = read_word();
  uint8_t value = processor.registers.y & 0xf;
  processor.clock_ticks += 5;
  processor.memory[location + processor.registers.x] = value;
}

static void XAS(void) {
  uint16_t location = read_word();
  uint8_t value = processor.registers.x & 0xf;
  processor.clock_ticks += 5;
  processor.memory[location + processor.registers.x] = value;
}

/* TODO */
static void AXA_absolutey(void) {
  uint16_t location = read_word();
  uint8_t written = processor.registers.accumulator & processor.registers.x &
                    ((location >> 8) + 1);
  processor.clock_ticks += 5;
  write_byte(written, location + processor.registers.y);
}

static void AXA_indirecty(void) {
  uint8_t address = read_byte();
  uint8_t written =
      processor.registers.accumulator & processor.registers.x & (address + 1);
  indirect_indexed(address);
  processor.clock_ticks += 6;
  write_byte(written, *value);
}

static void ANC(void) {
  uint8_t value = read_byte();
  processor.registers.accumulator &= value;
  processor.clock_ticks += 2;
  processor.registers._status.c = processor.registers.accumulator & 0x80;
}

static void LAS(void) {
  uint16_t location = read_word();
  processor.registers.accumulator =
      read_byte_at(location + processor.registers.y) & peek_from_stack();
  processor.clock_ticks += 4 + page_check(location, processor.registers.y);
  negative_check(processor.registers.accumulator);
  zero_check(processor.registers.accumulator);
}

#endif
