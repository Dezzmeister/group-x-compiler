#include "tac.h"
class CodeGenerator {
    // The code generator should take a series of 
    // three address code basic blocks and generate 
    // assembly for the target machine.
    // See Aho 8.6


    /* Code generator produces x86 assembly conforming to 
    the GAS (GNU Assembler) syntax. */
    BasicBlock * blocks; 
    CodeGenerator(BasicBlock * blocks) : blocks(blocks) {}
};
