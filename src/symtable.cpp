#include "symtable.h"

SymbolTable * x::default_symtable() {
  SymbolTable * out = new SymbolTable(nullptr);

  // These are the same in memory, they only exist to make the meaning clear
  Decl base_type = { .typ = nullptr };
  Decl base_func = { .func = nullptr };

  // Built-in types
  out->put(std::string("int"), new Symbol(Type, base_type));
  out->put(std::string("float"), new Symbol(Type, base_type));
  out->put(std::string("bool"), new Symbol(Type, base_type));
  out->put(std::string("char"), new Symbol(Type, base_type));
  out->put(std::string("void"), new Symbol(Type, base_type));

  // Built-in functions
  out->put(std::string("print"), new Symbol(Func, base_func));

  return out;
}

void x::create_scope(SymbolTable ** table) {
  SymbolTable * next = new SymbolTable(*table);
  *table = next;
}

SymbolTable * x::pop_scope(SymbolTable ** table) {
  SymbolTable * out = *table;
  *table = (*table)->enclosing;

  return out;
}
