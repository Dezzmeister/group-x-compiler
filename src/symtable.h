#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <string.h>
#include "parser.h"
#include <unordered_map>

union Value {
    int i;
    float f;
    char c;
    char * c_str;
};

class Symbol {
    public:
        int type;
        char * name;
        Value value;
        Symbol * next;
        Symbol(int type, Value value) : type(type), value(value) {}
        Symbol() {}
};

class SymbolTable {
    public:
        std::unordered_map<std::string, Symbol *> table;
        SymbolTable * enclosing;
    
        void Put(std::string name, Symbol * symbol) {
            table[name] = symbol;
        }
    
        Symbol * Get(std::string name) {
            return table[name];
        }
};
extern SymbolTable * symtable;
#endif
