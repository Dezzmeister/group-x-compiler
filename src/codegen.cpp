#include "codegen.h"

#include "parser.h"

CodeGenerator::CodeGenerator(const BasicBlock * b) : blocks(b) {}

CodeGenerator::~CodeGenerator() {
    delete blocks;
}
