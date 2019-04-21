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

extern void interpret_opcode(void);
extern void initialize_cpu(const unsigned char *data, size_t size, memory_map *m, processor_registers *reg);


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
void AND_help(uint8_t value);
void AND_im(void);
void AND_zero(void);
void AND_zerox(void);
void AND_absolute(void);
void AND_absolutex(void);
void AND_absolutey(void);
void AND_indirectx(void);
void AND_indirecty(void);
void ASL_help(uint8_t *address);
void ASL_accumulator(void);
void ASL_zero(void);
void ASL_zerox(void);
void ASL_absolute(void);
void ASL_absolutex(void);
void BCC(void);
void BCS(void);
void BEQ(void);
void BIT_help(uint16_t bit_test);
void BIT_zero(void);
void BIT_absolute(void);
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
void cmp_help(unsigned char value, unsigned char reg);
void CMP_help(unsigned char value);
void CMP_im(void);
void CMP_zero(void);
void CMP_zerox(void);
void CMP_absolute(void);
void CMP_absolutex(void);
void CMP_absolutey(void);
void CMP_indirectx(void);
void CMP_indirecty(void);
void CPX_help(unsigned char value);
void CPX_im(void);
void CPX_zero(void);
void CPX_absolute(void);
void CPY_help(unsigned char value);
void CPY_im(void);
void CPY_zero(void);
void CPY_absolute(void);
void DEC_help(uint16_t location);
void DEC_zero(void);
void DEC_zerox(void);
void DEC_absolute(void);
void DEC_absolutex(void);
void DEX(void);
void DEY(void);
void EOR_help(uint8_t value);
void EOR_im(void);
void EOR_zero(void);
void EOR_zerox(void);
void EOR_absolute(void);
void EOR_absolutex(void);
void EOR_absolutey(void);
void EOR_indirectx(void);
void EOR_indirecty(void);
void INC_help(uint16_t location);
void INC_zero(void);
void INC_zerox(void);
void INC_absolute(void);
void INC_absolutex(void);
void INX(void);
void INY(void);
void JMP_absolute(void);
void JMP_indirect(void);
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
void LDY_im(void);
void LDY_zero(void);
void LDY_zerox(void);
void LDY_absolute(void);
void LDY_absolutex(void);
void LSR_help(uint8_t *value);
void LSR_accumulator(void);
void LSR_zero(void);
void LSR_zerox(void);
void LSR_absolute(void);
void LSR_absolutex(void);
void NOP(void);
void ORA_help(uint16_t value);
void ORA_im(void);
void ORA_zero(void);
void ORA_zerox(void);
void ORA_absolute(void);
void ORA_absolutex(void);
void ORA_absolutey(void);
void ORA_indirectx(void);
void ORA_indirecty(void);
void PHA(void);
void PHP(void);
void PLA(void);
void PLP(void);
void ROL_help(uint8_t *location);
void ROL_accumulator(void);
void ROL_zero(void);
void ROL_zerox(void);
void ROL_absolute(void);
void ROL_absolutex(void);
void ROR_help(uint8_t *location);
void ROR_accumulator(void);
void ROR_zero(void);
void ROR_zerox(void);
void ROR_absolute(void);
void ROR_absolutex(void);
void RTI(void);
void RTS(void);
void SBC_help(uint8_t amt);
void SBC_im(void);
void SBC_zero(void);
void SBC_zerox(void);
void SBC_absolute(void);
void SBC_absolutex(void);
void SBC_absolutey(void);
void SBC_indirectx(void);
void SBC_indirecty(void);
void SEC(void);
void SED(void);
void SEI(void);
void STA_zero(void);
void STA_zerox(void);
void STA_absolute(void);
void STA_absolutex(void);
void STA_absolutey(void);
void STA_indirectx(void);
void STA_indirecty(void);
void STX_zero(void);
void STX_zeroy(void);
void STX_absolute(void);
void STY_zero(void);
void STY_zerox(void);
void STY_absolute(void);
void TAX(void);
void TAY(void);
void TSX(void);
void TXA(void);
void TXS(void);
void TYA(void);

#endif // CPU_H
