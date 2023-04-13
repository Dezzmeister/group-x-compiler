#include "symtable.h"
Symbol::Symbol(SymbolTable * table) {
        // Symbol table entry points to another symbol table
        // This occurs when parsing functions and structures.
        // The local variables and members of these constructs 
        // have their own scope.
        this->value.symtable = table;
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
