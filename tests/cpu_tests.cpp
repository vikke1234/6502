#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../headers/cpu.h"

void run_addressing_test(int start, addressing_modes_t mode) {
  for (int i = start; i < start + 0xe0; i += 0x20) {
    addressing_modes_t returned_mode = decode_addressing_mode(i);
    INFO("opcode: " << std::hex << i << ", mode returned: " << print_addressingmode(returned_mode) << ", comapred to: " << print_addressingmode(mode));
    CHECK(returned_mode == mode);
  }
}


TEST_CASE("01 indirect X", "[indirect X][indirect][01]"){
  run_addressing_test(0x01, INDIRECT_X);
}

TEST_CASE("01 zero page", "[zero page][01]"){
  run_addressing_test(0x05, ZERO_PAGE);
}

TEST_CASE("01 immediate", "[immediate][01]"){
  run_addressing_test(0x09, IMMEDIATE);
}

TEST_CASE("01 absolute", "[absolute][01]"){
  run_addressing_test(0x0d, ABSOLUTE);
}

TEST_CASE("01 indirect Y", "[indirect Y][indirect][01]"){
  run_addressing_test(0x11, INDIRECT_Y);
}

TEST_CASE("01 zero page X", "[zero page X][zero page][01]"){
  run_addressing_test(0x15, ZERO_PAGE_X);
}

TEST_CASE("01 absolute Y", "[absolute Y][absolute][01]"){
  run_addressing_test(0x19, ABSOLUTE_Y);
}

TEST_CASE("01 absolute X", "[absolute X][absolute][01]"){
  run_addressing_test(0x1d, ABSOLUTE_X);
}


TEST_CASE("10 immediate", "[immediate][10]") {
  /* TODO make this fail on all except LDX */
  run_addressing_test(0x2, IMMEDIATE);
}

TEST_CASE("10 zero page", "[zero page][10]") {
  run_addressing_test(0x6, ZERO_PAGE);
}

TEST_CASE("10 accumulator", "[accumulator][10]") {
  run_addressing_test(0xa, ACCUMULATOR);
}

TEST_CASE("10 absolute", "[absolute][10]") {
  run_addressing_test(0xe, ABSOLUTE);
}

TEST_CASE("10 zero page X", "[zero page X][zero page][10]"){
  run_addressing_test(0x16, ZERO_PAGE_X);
}

TEST_CASE("10 absolute X", "[absolute X][absolute][10]") {
  run_addressing_test(0x1e, ABSOLUTE_X);
}


TEST_CASE("00 immediate", "[immediate][00]"){
  run_addressing_test(0xa0, IMMEDIATE);
}

TEST_CASE("00 zero page", "[zero page][00]") {
  run_addressing_test(0x24, ZERO_PAGE);
}

TEST_CASE("00 absolute", "[absolute][00]") {
  run_addressing_test(0x2c, ABSOLUTE);
}

TEST_CASE("00 zero page X", "[zero page X][zero page][00]") {
  REQUIRE(decode_addressing_mode(0x94) == ZERO_PAGE_X);
  REQUIRE(decode_addressing_mode(0xb4) == ZERO_PAGE_X);
}

TEST_CASE("00 absolute X", "[absolute][absolute X][00]") {
  REQUIRE(decode_addressing_mode(0xbc) == ABSOLUTE_X);
}
