#ifndef SRC_TAC_H
#define SRC_TAC_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "symtable.h"

std::string next_t();
std::string next_l();
std::string next_p();

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
  std::string id;
  T value;
  void print() const { std::cout << id << " = " << value; };
  Value(std::string id, T v) : id(id), value(v) {}
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

class IfTAC : public Quad {
  public:
  std::string relop;
  std::string lhs;
  std::string rhs;
  IfTAC(std::string op, std::string l, std::string r) : 
  relop(op), lhs(l), rhs(r) {}
  void print() const {
    std::cout << "if !(" << lhs << ' ' << relop << rhs << ")" ;
  };
};

class Label : public Quad {
  std::string label;
  Label(std::string l) : label(l) {}
  void print() const {
    std::cout << label;
  }
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

class CmpLiteralTAC : public Quad {
  public:
    std::string id;
    int literal;
    CmpLiteralTAC(std::string id, int literal) : id(id), literal(literal) {}
    void print() const;
};

class JneTAC : public Quad {
  public:
    std::string label;
    JneTAC(std::string label) : label(label) {}
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

class LabelTAC : public Quad {
  public:
    std::string label;

    LabelTAC(std::string label) : label(label) {};
    void print() const;
};

// jmp L
class JumpTAC : public Quad
{
public:
  std::string label;
  JumpTAC(std::string l = 0) : label(l) {}

  void set_jmp(int lbl)
  {
    label = lbl;
  }
  void print() const;
};

class PushTAC : public Quad {
  std::string id; 
  PushTAC(std::string i) : id(i) { }
  void print() const { std::cout << "push " << id; }
};

class CallTAC : public Quad
{
public:
  std::string fun;
  CallTAC(std::string f) : fun(f) {}
  void print() const;
};

// t0 = __retval. asm stage will replace with rax
class RetvalTAC : public Quad {
  public:
    std::string id;
    RetvalTAC(std::string id) : id(id) {}
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
