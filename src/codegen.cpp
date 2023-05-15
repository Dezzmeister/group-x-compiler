#include "codegen.h"

#include "parser.h"

NamesToNames x::symtable_to_names(NamesToNames * parent, SymbolTable * symtable) {
    NamesToNames out;

    for (auto &item : symtable->table) {
        out->name_map[item.first] = next_p();
    }

    out.parent = parent;

    return out;
}

std::optional<std::string> NamesToNames::get(std::string name) {
    if (name_map.count(name)) {
        return std::optional<std::string>(name_map[name]);
    } else if (parent != nullptr) {
        return parent->get(name);
    }

    return std::nullopt;
}

CodeGenerator::CodeGenerator(const BasicBlock * b) : blocks(b) {}

CodeGenerator::~CodeGenerator() {
    delete blocks;
}
