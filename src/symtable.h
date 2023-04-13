#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <string.h>
#include "parser.h"
union YYSTYPE;
class SymbolTable;

union Value {
    int i;
    float f;
    char c;
    char * c_str;
    SymbolTable * symtable;
};
class Symbol {
    public:
    int type;
    char * name;
    Value value;
    Symbol * next;
    Symbol(int type, Value value) : type(type), value(value) {}
    Symbol() {}
    Symbol(SymbolTable * table);
};

class SymbolTable {
    public:
    SymbolTable * enclosing;
    Symbol * symbol_list;

    Symbol * PutSymbol(const char * name, int type) {
        Symbol * symbol = new Symbol();
        symbol->name = strdup(name);
        symbol->type = type;
        symbol->next = symbol_list;
        symbol_list = symbol;
        return symbol;
    }

    Symbol * PutSymbol(Symbol * symbol) {
        symbol->next = symbol_list;
        symbol_list = symbol;
        return symbol;
    }

    Symbol * GetSymbol(const char * name) {
        for (Symbol *p = symbol_list; p; p = p->next) {
            if (strcmp (p->name, name) == 0) {
                return p;
            }
        }
        return nullptr;
    }
    static void Push(SymbolTable * new_table);
    static SymbolTable * Pop();
};
extern SymbolTable * symtable;
#endif
