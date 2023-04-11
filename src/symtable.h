#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <string.h>
#include "parser.h"
union YYSTYPE;
class SymbolTable;
class Symbol {
    public:
    int type;
    char * name;
    YYSTYPE value;
    Symbol * next;
    Symbol(int type, YYSTYPE value) : type(type), value(value) {}
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
