#ifndef CODEGEN_H
#define CODEGEN_H

#include <map>

#include "ast.h"
#include "tac.h"

/**
 * This class owns all Typename ptrs, so be sure to call clone() before inserting
 * a typename into the map
 */
class TypeTable {
    std::map<std::string, Typename *> types;

    TypeTable() : types({}) {}

    ~TypeTable() {
        for (std::pair<const std::string, Typename *> &item : types) {
            delete item.second;
        }
    }

    void put(std::string name, Typename * typ) {
        types[name] = typ;
    }

    Typename * get(std::string name) {
        if (types.count(name)) {
            return types[name];
        }

        return nullptr;
    }
};

class CodeGenerator {

    void generate_code(const BasicBlock * b);

    public:
        const BasicBlock * blocks;
        CodeGenerator(const BasicBlock * b);
        ~CodeGenerator();
};
#endif
