#include "catch.hpp"
#include "../headers/cpu.h"

TEST_CASE("Initializing the CPU", "[init]") {
  memory_map memory;
  processor_registers registers;
  unsigned char instructions[] = {};
  initialize_cpu(instructions, sizeof(instructions), &memory, &registers);
  REQUIRE(registers.pc == 0x600);
}

TEST_CASE("ADC", "[ADC]") {
  memory_map memory;
  processor_registers registers;
  unsigned char instructions[] = {
    0xa9, 0x1,
    0x69, 0x1,
    0xa2, 0x2,
    0x86, 0x2,
    0x65, 0x2
  };
  initialize_cpu(instructions, sizeof(instructions), &memory, &registers);

  SECTION("Immediate") {
    interpret_opcode();
    REQUIRE(registers.accumulator == 0x1);
    interpret_opcode();
    REQUIRE(registers.accumulator == 0x2);
  }

  SECTION("Zero") {
    interpret_opcode();
    REQUIRE(registers.accumulator == 0x4);
  }
}
