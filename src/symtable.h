#ifndef SRC_SYMTABLE_H
#define SRC_SYMTABLE_H

#include <string.h>
#include <unordered_map>
#include "parser.h"

union Value {
    int i;
    float f;
    char c;
    char * c_str;
};

enum SymbolKind {
    VAR,
    TYPE,
    FUNC
};

class Symbol {
    public:
        SymbolKind type;
        char * name;
        Value value;
        Symbol * next;
        Symbol(SymbolKind type, Value value) : type(type), value(value) {}
        ~Symbol() {}
};

class SymbolTable {
    public:
        std::unordered_map<std::string, Symbol *> table;
        SymbolTable * enclosing;

        // SymbolTable does not take ownership of `enclosing` and enclosing table
        // should not be destroyed when this table is
        SymbolTable(SymbolTable * enclosing) : enclosing(enclosing) {}

        void put(std::string name, Symbol * symbol) {
            table[name] = symbol;
        }

        /**
         * Walk up the symbol table linked list and look for the symbol. If it is not in this
         * scope, it should be in an enclosing scope.
         */
        Symbol * get(std::string name) {
            // If `name` is not in the map, then using the [] operator will insert
            // a null value into the map. There was also no map::contains until C++20 :|
            if (table.count(name)) {
                return table[name];
            } else if (enclosing != nullptr) {
                return enclosing->get(name);
            }

            return nullptr;
        }
};

namespace x {
    extern SymbolTable * symtable;
}

#endif
