#ifndef SRC_SYMTABLE_H
#define SRC_SYMTABLE_H

#include <string.h>

#include <unordered_map>
#include <utility>

enum SymbolKind { Var, Type, Func };

class Symbol {
 public:
  SymbolKind type;
  char *name;
  Symbol *next;
  Symbol(SymbolKind type) : type(type) {}
  ~Symbol() {}
};

class SymbolTable {
 public:
  std::unordered_map<std::string, Symbol *> table;
  SymbolTable *enclosing;

  // SymbolTable does not take ownership of `enclosing` and enclosing table
  // should not be destroyed when this table is destroyed
  SymbolTable(SymbolTable *enclosing) : enclosing(enclosing) {}

  ~SymbolTable() {
    for (std::pair<const std::string, Symbol *> &item : table) {
      delete item.second;
    }
  }

  void put(std::string name, Symbol *symbol) { table[name] = symbol; }

  /**
   * Walk up the symbol table linked list and look for the symbol. If it is not
   * in this scope, it should be in an enclosing scope.
   */
  Symbol *get(std::string name) {
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
extern SymbolTable *symtable;

// Creates a new scope and returns the corresponding symtable
SymbolTable *create_scope();

// Pops the current scope and returns it. It is the caller's responsibility to
// free this memory
SymbolTable * pop_scope();
}  // namespace x

#endif
