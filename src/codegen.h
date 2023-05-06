#ifndef CODEGEN_H
#define CODEGEN_H
#include "tac.h"
class CodeGenerator {

    void generate_code(const BasicBlock * b);

    public:
        const BasicBlock * blocks;
        CodeGenerator(const BasicBlock * b);
        ~CodeGenerator();
};
#endif
