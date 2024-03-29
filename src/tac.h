#ifndef SRC_TAC_H
#define SRC_TAC_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "asm.h"
#include "symtable.h"

std::string next_t();
std::string next_l();
std::string next_p();

class TypeTable;
class NamesToNames;

class Quad
{
public:
  virtual void print() const;
  virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const = 0;

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
  virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class AssignTAC : public Quad
{
public:
  std::string id;
  std::string rhs;
  AssignTAC(std::string id, std::string rhs)
      : id(id), rhs(rhs) {}
  void print() const;
  virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

// artificial instruction that tells assembler that we can clean up this variable
class DeleteTAC : public Quad {
  public:
    std::string id;
    DeleteTAC(std::string id) : id(id) {};
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class CmpLiteralTAC : public Quad {
  public:
    std::string id;
    int literal;
    CmpLiteralTAC(std::string id, int literal) : id(id), literal(literal) {}
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class JneTAC : public Quad {
  public:
    std::string label;
    JneTAC(std::string label) : label(label) {}
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class LogicalTAC : public Quad {
  public:
    std::string id;
    std::string op;
    std::string left;
    std::string right;

    LogicalTAC(std::string id, std::string op, std::string left, std::string right) : id(id), op(op), left(left), right(right) {}
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class MathTAC : public Quad {
  public:
    std::string id;
    char op;
    std::string left;
    std::string right;

    MathTAC(std::string id, char op, std::string left, std::string right) :
      id(id), op(op), left(left), right(right) {}
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class LabelTAC : public Quad {
  public:
    std::string label;

    LabelTAC(std::string label) : label(label) {};
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class PushTAC : public Quad {
  public:
    std::string id; 
    PushTAC(std::string i) : id(i) { }
    void print() const { std::cout << "push " << id; }
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class SetupStackTAC : public Quad {
  public:
    SetupStackTAC() {};
    void print() const;
    virtual void to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class CallTAC : public Quad
{
public:
  std::string fun;
  CallTAC(std::string f) : fun(f) {}
  void print() const;
  virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

// t0 = __retval. asm stage will replace with rax
class RetvalTAC : public Quad {
  public:
    std::string id;
    RetvalTAC(std::string id) : id(id) {}
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

// arg x
class ArgTAC : public Quad
{
public:
  std::string id;
  int arg;
  std::string func;
  ArgTAC(std::string id, int a, std::string func) : id(id), arg(a), func(func) {}
  void print() const;
  virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

class VoidReturnTAC : public Quad {
  public:
    VoidReturnTAC() {}
    void print() const;
    virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
};

// mov(suffix) %eax
// ret
class ReturnTAC : public Quad
{
public:
  std::string id;

  ReturnTAC(std::string id) : id(id) {}
  void print() const;
  virtual void to_asm(std::ostream &out, TypeTable * type_table, NamesToNames &names, AsmState &state) const;
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

namespace x {
  extern BasicBlock *bblock;
}

#endif
