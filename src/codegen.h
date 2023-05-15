#ifndef CODEGEN_H
#define CODEGEN_H

#include "tac.h"
#include "map"

class NamestoNames {
    public:
    NamestoNames * parent = nullptr; 
    std::map<std::string, std::string> name_map;
};

NamestoNames * var_names_to_typenames(SymbolTable * symtable) {
    NamestoNames * n_to_n = new NamestoNames();
    for (auto& item: symtable->table) {
        n_to_n->name_map[item.first] = "p" + next_t();
    }
    return n_to_n;
}


class CodeGenerator {

    void generate_code(const BasicBlock * b);

    public:
        const BasicBlock * blocks;
        CodeGenerator(const BasicBlock * b);
        ~CodeGenerator();
};
#endif
