#include "codegen.h"
CodeGenerator::CodeGenerator(const BasicBlock * b) : blocks(b) {}

CodeGenerator::~CodeGenerator() {
    delete blocks;
}
