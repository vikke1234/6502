#ifndef CPU_H
#define CPU_H
#include <stdint.h>

void interpret_opcode(uint8_t opcode);
typedef void (*instruction_pointer)(uint8_t);

#endif // CPU_H
