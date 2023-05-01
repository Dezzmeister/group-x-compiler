#ifndef CODEGEN_H
#define CODEGEN_H
#include "tac.h"
class CodeGenerator {
        // See Aho 8.6
        // The code generator should take a series of
        // three address code basic blocks and generate
        // assembly for the target machine.

        /* The code generator produces x86 assembly conforming to
        the GAS (GNU Assembler) syntax. */
    public:
        const BasicBlock * blocks;
        CodeGenerator(const BasicBlock * b);
        ~CodeGenerator();
};
#endif
