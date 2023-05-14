#ifndef SRC_TAC_H
#define SRC_TAC_H

#include <cstdint>

#include "ast.h"
#include "symtable.h"

std::string next_t();

class Quad
{
public:
  virtual void print() const;

protected:
  virtual ~Quad() = default;
};

template <typename T>
class Value : public Quad
{
public:
  T value;
  void print() const { std::cout << next_t() << " = " << value; };
  Value(T v) : value(v) {}
};

class AssignTAC : public Quad
{
public:
  std::string op;
  Quad *lhs, *rhs;
  AssignTAC(std::string o, Quad * l, Quad * r)
      : op(o), lhs(l), rhs(r) {}
  void print() const;
};

class UnaryTAC : public Quad
{
public:
  std::string op;
  Quad * location;
  UnaryTAC(std::string o, Quad * loc) : op(o), location(loc) {}
};

class LoadTAC : public Quad
{
public:
  std::string identifier;
  LoadTAC(std::string id) : identifier(id) {}

  void print() const;
};

class MoveTAC : public Quad
{
public:
  Quad * location;
  std::string ident;
  MoveTAC(std::string id, Quad * loc) : location(loc), ident(id) {}

  void print() const;
};

// x = y
class CopyTAC : public Quad
{
public:
  Quad * location;
  CopyTAC(Quad * loc) : location(loc) {}
  void print() const;
};

// x relop y
// jne L
class CondJumpRelopTAC : public Quad
{
public:
  char op;
  int lhs, rhs;
  int label;
  CondJumpRelopTAC(int l, int r, int lbl)
      : lhs(l), rhs(r), label(lbl) {}

  void set_jmp(int lbl)
  {
    label = lbl;
  }
};
// if x goto L
class CondJumpTAC : public Quad
{
public:
  Quad * location;
  int jmp_label;
  CondJumpTAC(Quad * loc, int label = 0)
      : location(loc), jmp_label(label) {}

  void print() const;
  void set_jmp(int label)
  {
    jmp_label = label;
  }

};

// jmp L
class JumpTAC : public Quad
{
public:
  int label;
  JumpTAC(int l = 0) : label(l) {}

  void set_jmp(int lbl)
  {
    label = lbl;
  }
  void print() const;
};

// arg 1
// arg 2
// call f 2
class CallTAC : public Quad
{
public:
  std::string fun;
  int num_args;
  CallTAC(std::string f, int n) : fun(f), num_args(n) {}
  void print() const;
};

// arg x
class ArgTAC : public Quad
{
public:
  int arg;
  ArgTAC(int a) : arg(a) {}
};

// x = y[i]
class IndexCopyTAC : public Quad
{
public:
  const Symbol *arr;
  Quad * location;
  IndexCopyTAC(const Symbol *a, Quad * loc) : arr(a), location(loc) {}
};

// x[i] = y
class ValueCopyTAC : public Quad
{
public:
  const Symbol *arr;
  Quad * location;
  std::string value;
  ValueCopyTAC(const Symbol *a, Quad * loc) : arr(a), location(loc) {}

  void print() const;
};

// x = &y
class AddrTac : public Quad
{
public:
  Quad * location;
  AddrTac(Quad * loc) : location(loc) {}
};

// x = *y
class DerefTAC : public Quad
{
public:
  Quad * location;
  DerefTAC(Quad * loc) : location(loc) {}
};
// *x = y
class AddrSetTAC : public Quad
{
public:
  Quad * location;
  int value;
  AddrSetTAC(Quad * loc, int v) : location(loc), value(v) {}
};

class CastTAC : public Quad
{
public:
  Quad * location;
  int source_kind;
  int destination_kind;
  CastTAC(Quad * loc, int src, int dest)
      : location(loc), source_kind(src), destination_kind(dest) {}
};

// mov(suffix) %eax
// ret
class ReturnTAC : public Quad
{
public:
  int ret_val;

  ReturnTAC(int ret) : ret_val(ret) {}
};

class BasicBlock
{
public:
  std::vector<Quad *> trips = std::vector<Quad *>();

  std::string name;

  int n_instructions = 0;

  void add_block(std::string name);

  void add_instruction(Quad *trip)
  {
    trips.push_back(trip);
    ++n_instructions;
  }

  int last_instruction() const { return n_instructions - 1; }

  int next_instruction() const { return n_instructions; }

  Quad * get_instruction(const ASTNode &n);

  void print() const;
  BasicBlock() = default;

  BasicBlock(std::string str) : name(str) {}

  BasicBlock *prev = nullptr;
};

namespace x
{
  extern BasicBlock *bblock;
}

#endif
