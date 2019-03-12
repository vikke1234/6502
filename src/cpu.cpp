#include "../headers/cpu.h"
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <unordered_map>

/* this is for editing processor status */
typedef enum {
	NEGATIVE    = 0x1 << 6,
	OVERFLOW    = 0x1 << 5,
	BFLAG       = 0x1 << 4,
	DECIMAL     = 0x1 << 3,
	INTERRUPT   = 0x1 << 2,
	ZERO        = 0x1 << 1,
	CARRY       = 0x1 << 0
} flags_t;

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

/* this will probably just be written to as a uint8_t * instead of separately */
typedef struct {
	uint8_t RAM              [0x1fff]; /* RAM size 0x800, mirrored 3 times */
	uint8_t ppu_registers    [0x2000]; /* actual size 0x8, repeats every 8 bytes */
	uint8_t apu_registers    [0x18];
	uint8_t test_registers   [0x8];    /* for when the CPU is in test mode */
	uint8_t rom              [0xbfe0]; /* ROM space and mapper registers */
} memory_map;

typedef struct {
	uint16_t pc;
	uint8_t accumulator;
	uint8_t stack_pointer; /* this probably has to be an array */
	uint8_t x, y;
	uint8_t status;          /** NVss DIZC, @see FLAGS */
} processor_registers;

static processor_registers registers;
static memory_map memory;


/**
 * @brief sets the flag to the value of b
 * @param flag
 * @param b what the flag should be
 */

static void set_flags(flags_t flag, bool b);

/**
 * @brief gets a given flag, 1 or 0
 * @param flag
 */

static uint8_t get_flag(flags_t flag);

/**
 * @brief reads 2 little endian  bytes and combines them into a 16bit unsigned
 * assumes that thereemacs's atleast 2 bytes to read
 */
static uint16_t read2bytes();

/**
 * @brief decodes what addressing mode the opcode is in
 * @param opcode
 */

static addressing_modes_t decode_opcode(uint8_t opcode);
/**
 * @brief prints which addressing mode is currently used
 * @param mode
 */
static const char *print_addressingmode (addressing_modes_t mode);

/**
 * @brief does comparison for branch instructions
 * @param opcode
 * @return true/false whether registercompared is equal to the 6th byte
 */
static bool compare (uint8_t opcode);

/* TODO: add small documentation on theese, maybe not and just link documentation, maybe remove static?
 * also think how to get theese to a map in main.cpp, theese might be removed,
 * they'll stay for now untill I decide how I should implement a jump table, if I even do */
static void ADC(addressing_modes_t addressing_mode);
static void AND(addressing_modes_t addressing_mode);
static void ASL(addressing_modes_t addressing_mode);
static void BCC(addressing_modes_t addressing_mode);
static void BCS(addressing_modes_t addressing_mode);
static void BEQ(addressing_modes_t addressing_mode);
static void BIT(addressing_modes_t addressing_mode);
static void BMI(addressing_modes_t addressing_mode);
static void BNE(addressing_modes_t addressing_mode);
static void BPL(addressing_modes_t addressing_mode);
static void BRK(addressing_modes_t addressing_mode);
static void BVC(addressing_modes_t addressing_mode);
static void BVS(addressing_modes_t addressing_mode);
static void CLC(addressing_modes_t addressing_mode);
static void CLD(addressing_modes_t addressing_mode);
static void CLI(addressing_modes_t addressing_mode);
static void CLV(addressing_modes_t addressing_mode);
static void CMP(addressing_modes_t addressing_mode);
static void CPX(addressing_modes_t addressing_mode);
static void CPY(addressing_modes_t addressing_mode);
static void DEC(addressing_modes_t addressing_mode);
static void DEX(addressing_modes_t addressing_mode);
static void DEY(addressing_modes_t addressing_mode);
static void EOR(addressing_modes_t addressing_mode);
static void INC(addressing_modes_t addressing_mode);
static void INX(addressing_modes_t addressing_mode);
static void INY(addressing_modes_t addressing_mode);
static void JMP(addressing_modes_t addressing_mode);
static void JSR(addressing_modes_t addressing_mode);
static void LDA(addressing_modes_t addressing_mode);
static void LDX(addressing_modes_t addressing_mode);
static void LDY(addressing_modes_t addressing_mode);
static void LSR(addressing_modes_t addressing_mode);
static void NOP(addressing_modes_t addressing_mode);
static void ORA(addressing_modes_t addressing_mode);
static void PHA(addressing_modes_t addressing_mode);
static void PHP(addressing_modes_t addressing_mode);
static void PLA(addressing_modes_t addressing_mode);
static void PLP(addressing_modes_t addressing_mode);
static void ROL(addressing_modes_t addressing_mode);
static void ROR(addressing_modes_t addressing_mode);
static void RTI(addressing_modes_t addressing_mode);
static void RTS(addressing_modes_t addressing_mode);
static void SBC(addressing_modes_t addressing_mode);
static void SEC(addressing_modes_t addressing_mode);
static void SED(addressing_modes_t addressing_mode);
static void SEI(addressing_modes_t addressing_mode);
static void STA(addressing_modes_t addressing_mode);
static void STX(addressing_modes_t addressing_mode);
static void STY(addressing_modes_t addressing_mode);
static void TAX(addressing_modes_t addressing_mode);
static void TAY(addressing_modes_t addressing_mode);
static void TSX(addressing_modes_t addressing_mode);
static void TXA(addressing_modes_t addressing_mode);
static void TXS(addressing_modes_t addressing_mode);
static void TYA(addressing_modes_t addressing_mode);

extern void interpret_opcode(uint8_t opcode)
{
	addressing_modes_t addressing_mode = RELATIVE;

	if(opcode & 0x10) {
		addressing_mode = decode_opcode(opcode);
		print_addressingmode(addressing_mode);
	}
	switch (opcode) {
	/* ASL - Arithmetic Shift Left*/
	case 0x69:
	case 0x65:
	case 0x75:
	case 0x6D:
	case 0x7D:
	case 0x79:
	case 0x61:
	case 0x71:
		break;


	/* BCS - Branch if Carry Set*/
	case 0x29:
	case 0x25:
	case 0x35:
	case 0x2D:
	case 0x3D:
	case 0x39:
	case 0x21:
	case 0x31:
		break;


	/* BIT - Bit Test*/
	case 0x0A:
	case 0x06:
	case 0x16:
	case 0x0E:
	case 0x1E:
		break;


	/* BNE - Branch if Not Equal*/
	case 0x90:
		break;


	/* BRK - Force Interrupt*/
	case 0xB0:
		break;


	/* BVS - Branch if Overflow Set*/
	case 0xF0:
		break;


	/* CLD - Clear Decimal Mode*/
	case 0x24:
	case 0x2C:
		break;


	/* CLV - Clear Overflow Flag*/
	case 0x30:
		break;


	/* CPX - Compare X Register*/
	case 0xD0:
		break;


	/* DEC - Decrement Memory*/
	case 0x10:
		break;


	/* DEY - Decrement Y Register*/
	case 0x00:
		break;


	/* INC - Increment Memory*/
	case 0x50:
		break;


	/* INY - Increment Y Register*/
	case 0x70:
		break;


	/* JSR - Jump to Subroutine*/
	case 0x18:
		break;


	/* LDX - Load X Register*/
	case 0xD8:
		break;


	/* LSR - Logical Shift Right*/
	case 0x58:
		break;


	/* ORA - Logical Inclusive OR*/
	case 0xB8:
		break;


	/* PHP - Push Processor Status*/
	case 0xC9:
	case 0xC5:
	case 0xD5:
	case 0xCD:
	case 0xDD:
	case 0xD9:
	case 0xC1:
	case 0xD1:
		break;


	/* PLP - Pull Processor Status*/
	case 0xE0:
	case 0xE4:
	case 0xEC:
		break;


	/* ROR - Rotate Right*/
	case 0xC0:
	case 0xC4:
	case 0xCC:
		break;


	/* RTS - Return from Subroutine*/
	case 0xC6:
	case 0xD6:
	case 0xCE:
	case 0xDE:
		break;


	/* SEC - Set Carry Flag*/
	case 0xCA:
		break;


	/* SEI - Set Interrupt Disable*/
	case 0x88:
		break;


	/* STX - Store X Register*/
	case 0x49:
	case 0x45:
	case 0x55:
	case 0x4D:
	case 0x5D:
	case 0x59:
	case 0x41:
	case 0x51:
		break;


	/* TAX - Transfer Accumulator to X*/
	case 0xE6:
	case 0xF6:
	case 0xEE:
	case 0xFE:
		break;


	/* TSX - Transfer Stack Pointer to X*/
	case 0xE8:
		break;


	/* TXS - Transfer X to Stack Pointer*/
	case 0xC8:
		break;

	default:
		break;
	}
}

static void set_flags (flags_t flag, bool b)
{
	if (get_flag(flag) != b) {
		registers.status ^= flag;
	}
}

static uint8_t get_flag(flags_t flag)
{
	return !!(registers.status & flag); /* !! to turn it into a 1 or 0 to not require shifting and looking up the power of two it is */
}

static uint16_t read2bytes()
{
	return ((memory.rom[registers.pc + 1] << 8) | memory.rom[registers.pc]);
}

static addressing_modes_t decode_opcode(uint8_t opcode)
{
	static std::unordered_map<int, addressing_modes_t> modes10 = {
		{0x0, IMMEDIATE},
		{0x1, ZERO_PAGE},
		{0x2, ACCUMULATOR},
		{0x3, ABSOLUTE},
		{0x5, ZERO_PAGE_X},
		{0x7, ABSOLUTE_X}
	};
	static std::unordered_map<int, addressing_modes_t> modes01 = {
		{0x0, INDIRECT_X},
		{0x1, ZERO_PAGE},
		{0x2, IMMEDIATE},
		{0x3, ABSOLUTE},
		{0x4, INDIRECT_Y},
		{0x5, ZERO_PAGE_X},
		{0x6, ABSOLUTE_Y},
		{0x7, ABSOLUTE_X}
	};
	static std::unordered_map<int, addressing_modes_t> modes00 = {
		{0x0, IMMEDIATE},
		{0x1, ZERO_PAGE},
		{0x3, ABSOLUTE},
		{0x5, ZERO_PAGE_X},
		{0x7, ABSOLUTE_X}
	};

	/* this is done to check which type of opcode
	 * we're dealing with as there's 4 different ways to decode it*/
	const uint8_t type = opcode & 0x3;
	const uint8_t mode = (opcode & 0x1c) >> 2;

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

static const char *print_addressingmode (addressing_modes_t mode)
{
	const char * const modes[] = {
		"IMPLICT",
		"ACCUMULATOR",
		"IMMEDIATE",
		"ZERO_PAGE",
		"ZERO_PAGE_X",
		"ZERO_PAGE_Y",
		"RELATIVE",
		"IMPLIED",
		"ABSOLUTE",
		"ABSOLUTE_X",
		"ABSOLUTE_Y",
		"INDIRECT",
		"INDIRECT_X",
		"INDIRECT_Y",
		"UNKNOWN"
	};
  return modes[mode];
}
/* TODO add tests for this and decode addressing mode */
static bool compare (uint8_t opcode)
{
	uint8_t compare_to = (opcode & 0x20) >> 5;
	/* we need to get the first two bytes which decides which register to compare */
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
