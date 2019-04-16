#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include <stdlib.h>

extern void interpret_opcode();
extern void initialize_cpu(const unsigned char *data, size_t size);

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

/* TODO: add small documentation on theese, maybe not and just link documentation */

void ADC_help(uint16_t value);

void ADC_im(void);
void ADC_zero(void);
void ADC_zerox(void);
void ADC_zeroy(void);
void ADC_absolute(void);
void ADC_absolutex(void);
void ADC_absolutey(void);
void ADC_indirectx(void);
void ADC_indirecty(void);

void AND(void);
void ASL(void);
void BCC(void);
void BCS(void);
void BEQ(void);
void BIT(void);
void BMI(void);
void BNE(void);
void BPL(void);
void BRK(void);
void BVC(void);
void BVS(void);
void CLC(void);
void CLD(void);
void CLI(void);
void CLV(void);
void CMP(void);
void CPX(void);
void CPY(void);
void DEC(void);
void DEX(void);
void DEY(void);
void EOR(void);
void INC(void);
void INX(void);
void INY(void);
void JMP(void);
void JSR(void);

void LDA_help(uint16_t value);
void LDA_im(void);
void LDA_zero(void);
void LDA_zerox(void);
void LDA_absolute(void);
void LDA_absolutex(void);
void LDA_absolutey(void);
void LDA_indirectx(void);
void LDA_indirecty(void);

void LDX_help(uint8_t value);
void LDX_im(void);
void LDX_zero(void);
void LDX_zeroy(void);
void LDX_absolute(void);
void LDX_absolutey(void);

void LDY_help(uint8_t value);
void LDY_im (void);
void LDY_zero(void);
void LDY_zerox(void);
void LDY_absolute(void);
void LDY_absolutex(void);

void LSR(void);
void NOP(void);
void ORA(void);
void PHA(void);
void PHP(void);
void PLA(void);
void PLP(void);
void ROL(void);
void ROR(void);
void RTI(void);
void RTS(void);
void SBC(void);
void SEC(void);
void SED(void);
void SEI(void);
void STA(void);
void STX(void);
void STY(void);
void TAX(void);
void TAY(void);
void TSX(void);
void TXA(void);
void TXS(void);
void TYA(void);

#endif // CPU_H
