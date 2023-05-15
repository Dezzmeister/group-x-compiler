#ifndef CODEGEN_H
#define CODEGEN_H

#include <optional>

#include "tac.h"
#include "map"

class NamesToNames {
    public:
    NamesToNames * parent = nullptr; 
    std::map<std::string, std::string> name_map;

    std::optional<std::string> get(std::string name);
};

namespace x {
    NamesToNames symtable_to_names(NamesToNames * parent, SymbolTable * symtable);
}

class CodeGenerator {

    void generate_code(const BasicBlock * b);

    public:
        const BasicBlock * blocks;
        CodeGenerator(const BasicBlock * b);
        ~CodeGenerator();
};
#endif
