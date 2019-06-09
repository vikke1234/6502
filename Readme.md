# 6502 Emulator
This is to be compiled as an shared object/dll so it gets black boxed, 
there' functions to dump the registers i.e. `dump_registers()` and running the next instruction `interpret_opcode()`

## How to compile
`mkdir build && cd build`
`cmake ..`
`make`