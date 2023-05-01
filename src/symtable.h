#ifndef SRC_SYMTABLE_H
#define SRC_SYMTABLE_H

#include <string.h>

#include <unordered_map>
#include <utility>

enum SymbolKind { Var, Type, Func };

class VarDecl;
class FuncDecl;
class TypeDecl;

typedef union {
    VarDecl * var;
    FuncDecl * func;
    TypeDecl * typ;
} Decl;

class Symbol {
    public:
        SymbolKind kind;
        char * name;
        Decl decl;
        Symbol * next;
        Symbol(SymbolKind kind, Decl decl) : kind(kind), decl(decl) {}
        ~Symbol() {}
};

class SymbolTable {
    public:
        std::unordered_map<std::string, Symbol *> table;
        SymbolTable * enclosing;

        // SymbolTable does not take ownership of `enclosing` and enclosing table
        // should not be destroyed when this table is destroyed
        SymbolTable(SymbolTable * enclosing) : enclosing(enclosing) {}

        ~SymbolTable() {
            for (std::pair<const std::string, Symbol *> &item : table) {
                delete item.second;
            }
        }

        void put(std::string name, Symbol * symbol) {
            table[name] = symbol;
        }

        /**
         * Walk up the symbol table linked list and look for the symbol. If it is not
         * in this scope, it should be in an enclosing scope.
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
    // Creates a new scope and returns the corresponding symtable
    void create_scope(SymbolTable ** table);

    // Pops the current scope and returns it. It is the caller's responsibility to
    // free this memory
    SymbolTable * pop_scope(SymbolTable ** table);

    // Creates the default top-level symbol table with symbols for primitive types and
    // built in functions. It is the caller's responsibility to free this memory
    SymbolTable * default_symtable();
}   // namespace x

#endif
