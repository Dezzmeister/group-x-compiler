#include "symtable.h"
Symbol::Symbol(SymbolTable * table) {
        // Create a symbol for struct and function types. 
        for (Symbol * s = table->symbol_list; s; s = s->next) {
            this->type += s->type;
        }
    }
void SymbolTable::Push(SymbolTable * new_table) {
    new_table->enclosing = symtable;
    symtable = new_table;
}
SymbolTable * SymbolTable::Pop() {
    SymbolTable * table = symtable;
    symtable = symtable->enclosing;
    return table;
}
