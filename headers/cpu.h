#ifndef CPU_H
#define CPU_H
#include <stdint.h>

void interpret_opcode(uint8_t opcode);

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
  BFLAG       = 0x1 << 4,
  DECIMAL     = 0x1 << 3,
  INTERRUPT   = 0x1 << 2,
  ZERO        = 0x1 << 1,
  CARRY       = 0x1 << 0
} flags_t;


/* this will probably just be written to as a uint8_t * instead of separately */
typedef struct {
  unsigned char RAM              [0x1fff]; /* RAM size 0x800, mirrored 3 times */
  unsigned char ppu_registers    [0x2000]; /* actual size 0x8, repeats every 8 bytes */
  unsigned char apu_registers    [0x18];
  unsigned char test_registers   [0x8];    /* for when the CPU is in test mode */
  unsigned char rom              [0xbfe0]; /* ROM space and mapper registers */
} memory_map;

typedef struct {
  uint16_t pc;
  uint8_t accumulator;
  uint8_t stack_pointer[256];
  uint8_t _sp;
  uint8_t x, y;
  uint8_t status;          /** NVsB DIZC, @see FLAGS */
} processor_registers;

/* TODO maybe make like a "cputype" which contains information
 * about the instruction that was executed */

/**
 * @brief prints which addressing mode is currently used
 * @param mode
 */
static const char *print_addressingmode (addressing_modes_t mode);

/**
 * @brief sets the flag to the value of b
 * @param flag
 * @param b what the flag should be
 */

static inline void set_flags(flags_t flag, bool b);

/**
 * @brief takes an array of flags and checks for whether they should be set or not
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
 * @brief reads 2 little endian bytes and combines them into a 16bit unsigned
 * assumes that thereemacs's atleast 2 bytes to read, note increment PC by 2
 */
static inline uint16_t read_word();

/**
 * @brief reads 1 byte (little endian), note increment PC
 */
static inline uint8_t read_byte();

/**
 * @brief decodes what addressing mode the opcode is in
 * @param opcode
 */

static addressing_modes_t decode_addressing_mode(uint8_t opcode);

/**
 * @brief does comparison for branch instructions
 * @param opcode
 * @return true/false whether registercompared is equal to the 6th byte
 */
static bool compare (uint8_t opcode);

/* TODO: add small documentation on theese, maybe not and just link documentation, maybe remove static?
 * also think how to get theese to a map in main.cpp, theese might be removed,
 * they'll stay for now untill I decide how I should implement a jump table, if I even do */

void ADC(addressing_modes_t addressing_mode);
void AND(addressing_modes_t addressing_mode);
void ASL(addressing_modes_t addressing_mode);
void BCC(addressing_modes_t addressing_mode);
void BCS(addressing_modes_t addressing_mode);
void BEQ(addressing_modes_t addressing_mode);
void BIT(addressing_modes_t addressing_mode);
void BMI(addressing_modes_t addressing_mode);
void BNE(addressing_modes_t addressing_mode);
void BPL(addressing_modes_t addressing_mode);
void BRK(addressing_modes_t addressing_mode);
void BVC(addressing_modes_t addressing_mode);
void BVS(addressing_modes_t addressing_mode);
void CLC(addressing_modes_t addressing_mode);
void CLD(addressing_modes_t addressing_mode);
void CLI(addressing_modes_t addressing_mode);
void CLV(addressing_modes_t addressing_mode);
void CMP(addressing_modes_t addressing_mode);
void CPX(addressing_modes_t addressing_mode);
void CPY(addressing_modes_t addressing_mode);
void DEC(addressing_modes_t addressing_mode);
void DEX(addressing_modes_t addressing_mode);
void DEY(addressing_modes_t addressing_mode);
void EOR(addressing_modes_t addressing_mode);
void INC(addressing_modes_t addressing_mode);
void INX(addressing_modes_t addressing_mode);
void INY(addressing_modes_t addressing_mode);
void JMP(addressing_modes_t addressing_mode);
void JSR(addressing_modes_t addressing_mode);
void LDA(addressing_modes_t addressing_mode);
void LDX(addressing_modes_t addressing_mode);
void LDY(addressing_modes_t addressing_mode);
void LSR(addressing_modes_t addressing_mode);
void NOP(addressing_modes_t addressing_mode);
void ORA(addressing_modes_t addressing_mode);
void PHA(addressing_modes_t addressing_mode);
void PHP(addressing_modes_t addressing_mode);
void PLA(addressing_modes_t addressing_mode);
void PLP(addressing_modes_t addressing_mode);
void ROL(addressing_modes_t addressing_mode);
void ROR(addressing_modes_t addressing_mode);
void RTI(addressing_modes_t addressing_mode);
void RTS(addressing_modes_t addressing_mode);
void SBC(addressing_modes_t addressing_mode);
void SEC(addressing_modes_t addressing_mode);
void SED(addressing_modes_t addressing_mode);
void SEI(addressing_modes_t addressing_mode);
void STA(addressing_modes_t addressing_mode);
void STX(addressing_modes_t addressing_mode);
void STY(addressing_modes_t addressing_mode);
void TAX(addressing_modes_t addressing_mode);
void TAY(addressing_modes_t addressing_mode);
void TSX(addressing_modes_t addressing_mode);
void TXA(addressing_modes_t addressing_mode);
void TXS(addressing_modes_t addressing_mode);
void TYA(addressing_modes_t addressing_mode);

#endif // CPU_H
