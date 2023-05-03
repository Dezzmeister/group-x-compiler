#include "symtable.h"

Symbol * Symbol::clone() const {
    Symbol * out = new Symbol(kind, decl);
    out->next = next;
    out->name = name;

    return out;
}

SymbolTable * SymbolTable::clone() const {
    SymbolTable * out = new SymbolTable(enclosing == nullptr ? nullptr : enclosing->clone());
    out->node = node;

    for (auto &item : table) {
        out->table[item.first] = item.second->clone();
    }

    return out;
}

void SymbolTable::print() {
    for (auto &item : table) {
        printf("[%s -> %s]\n", item.first.c_str(), x::symbol_kind_names[item.second->kind]);
    }
}

SymbolTable * x::bare_symtable() {
    SymbolTable * out = new SymbolTable(nullptr);

    Decl base_type = { .typ = nullptr };

    // Built-in types
    out->put(std::string("int"), new Symbol(Type, base_type));
    out->put(std::string("float"), new Symbol(Type, base_type));
    out->put(std::string("bool"), new Symbol(Type, base_type));
    out->put(std::string("char"), new Symbol(Type, base_type));
    out->put(std::string("void"), new Symbol(Type, base_type));

    return out;
}

void x::create_scope(SymbolTable ** table) {
    SymbolTable * next = new SymbolTable(*table);
    *table = next;
}

SymbolTable * x::pop_scope(SymbolTable ** table) {
    SymbolTable * out = *table;
    *table = (*table)->enclosing;

    return out;
}
