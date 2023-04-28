#include "symtable.h"

static SymbolTable* init_symtable();

SymbolTable * x::symtable = init_symtable();

static SymbolTable * init_symtable() {
  SymbolTable * out = new SymbolTable(nullptr);

  // Built-in types
  out->put(std::string("int"), new Symbol(Type));
  out->put(std::string("float"), new Symbol(Type));
  out->put(std::string("bool"), new Symbol(Type));
  out->put(std::string("char"), new Symbol(Type));
  out->put(std::string("void"), new Symbol(Type));

  // Built-in functions
  out->put(std::string("print"), new Symbol(Func));

  return out;
}

SymbolTable * x::create_scope() {
  SymbolTable * out = new SymbolTable(x::symtable);
  x::symtable = out;

  return out;
}

SymbolTable * x::pop_scope() {
  SymbolTable * parent = x::symtable->enclosing;
  SymbolTable * out = x::symtable;

  x::symtable = parent;
  return out;
}
