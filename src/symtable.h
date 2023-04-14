#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <string.h>
#include "parser.h"
#include <map>

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
    std::map<char *, Symbol *> table;
    SymbolTable * enclosing;
};
extern SymbolTable * symtable;
#endif
