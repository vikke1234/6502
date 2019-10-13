# 6502 Emulator
This is to be compiled as an shared object/dll so it gets black boxed, 
there' functions to dump the registers i.e. `dump_registers()` and running the next instruction `interpret_opcode()`

## How to compile on linux
`mkdir build && cd build`
`cmake ..`
`make`

## How to compile on windows VS 2019

`cmake -G "Visual Studio 16 2019"`
Open solution in VS and build