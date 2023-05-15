#ifndef SRC_SYMTABLE_H
#define SRC_SYMTABLE_H

#include <string.h>

#include <unordered_map>
#include <utility>

enum SymbolKind { Var, Type, Func };

class ASTNode;
class VarDecl;
class FuncDecl;
class TypeDecl;

typedef union {
    VarDecl * var;
    TypeDecl * typ;
    FuncDecl * func;
} Decl;

class Symbol {
    public:
        SymbolKind kind;
        char * name;
        Decl decl;
        Symbol * next;
        bool initialized;
        Symbol(SymbolKind kind, Decl decl) : kind(kind), decl(decl), initialized(false) {}

        Symbol * clone() const;

        ASTNode * decl_upcast();

        ~Symbol() {}
};

class SymbolTable {
    public:
        std::unordered_map<std::string, Symbol *> table;
        SymbolTable * enclosing;
        // AST node that has this scope. The symbol table does not own this node
        ASTNode * node;

        // SymbolTable does not take ownership of `enclosing` and enclosing table
        // should not be destroyed when this table is destroyed
        SymbolTable(SymbolTable * enclosing) : enclosing(enclosing), node(nullptr) {}

        SymbolTable * clone() const;

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

        void set_node(ASTNode * node) {
            this->node = node;
        }

        void print();
};

namespace x {
    const char * const symbol_kind_names[] = {"Var", "Type", "Func"};

    // Creates a new scope and returns the corresponding symtable
    void create_scope(SymbolTable ** table);

    // Pops the current scope and returns it. It is the caller's responsibility to
    // free this memory
    SymbolTable * pop_scope(SymbolTable ** table);

    // Creates a bare top-level symbol table with symbols for primitive types only.
    // No built-in functions
    SymbolTable * bare_symtable();
}   // namespace x

#endif
