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
  int lhs, rhs;
  AssignTAC(std::string o, int l, int r)
      : op(o), lhs(l), rhs(r) {}
  void print() const;
};

class UnaryTAC : public Quad
{
public:
  std::string op;
  int index;
  UnaryTAC(std::string o, int i) : op(o), index(i) {}
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
  std::string location;
  int index;
  MoveTAC(std::string loc, int i) : location(loc), index(i) {}

  void print() const;
};

// x = y
class CopyTAC : public Quad
{
public:
  int index;
  CopyTAC(int i) : index(i) {}
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
  int ident_index;
  int jmp_label;
  CondJumpTAC(int idx, int label = 0)
      : ident_index(idx), jmp_label(label) {}

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
};

// arg 1
// arg 2
// call f 2
class CallTAC : public Quad
{
public:
  void *func;
  int num_args;
  std::string fun;
  CallTAC(void *f, int n) : func(f), num_args(n) {}
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
  int index;
  IndexCopyTAC(const Symbol *a, int i) : arr(a), index(i) {}
};

// x[i] = y
class ValueCopyTAC : public Quad
{
public:
  const Symbol *arr;
  int index;
  std::string value;
  ValueCopyTAC(const Symbol *a, int i) : arr(a), index(i) {}

  void print() const;
};

// x = &y
class AddrTac : public Quad
{
public:
  int index;
  AddrTac(int i) : index(i) {}
};

// x = *y
class DerefTAC : public Quad
{
public:
  int index;
  DerefTAC(int i) : index(i) {}
};
// *x = y
class AddrSetTAC : public Quad
{
public:
  int index;
  int value;
  AddrSetTAC(int i, int v) : index(i), value(v) {}
};

class CastTAC : public Quad
{
public:
  int index;
  int source_kind;
  int destination_kind;
  CastTAC(int i, int src, int dest)
      : index(i), source_kind(src), destination_kind(dest) {}
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

  int get_instruction(const ASTNode &n);

  int get_instruction(const ArrayLiteral & arr);

  void print() const;
  BasicBlock() = default;

  BasicBlock(std::string str) : name(str) {}

  BasicBlock *prev;
};

namespace x
{
  extern BasicBlock *bblock;
}

#endif
