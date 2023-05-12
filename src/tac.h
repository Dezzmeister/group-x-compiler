#ifndef SRC_TAC_H
#define SRC_TAC_H

#include <cstdint>

#include "ast.h"
#include "symtable.h"

class Quad {
 public:
  virtual void print() const {};

 protected:
  virtual ~Quad() = default;
};

template <typename T>
class Value : public Quad {
  public:
    T value;
    Value(T v) : value(v) {}
};

class AssignTAC : public Quad {
 public:
  const char op;
  int lhs, rhs;
  int kind;

  AssignTAC(const char  o, int l, int r, int k)
  : op(o), lhs(l), rhs(r), kind(k) {}

  void print() const {
  }

};

class UnaryTAC : public Quad {
 public:
  std::string op;
  int index;
  int kind;

  UnaryTAC(std::string o, int i, int k) : op(o), index(i), kind(k) {}

  void print() const {
    std::cout << index << " = " << op << index << std::endl;
  }
};

// x = y
class CopyTAC : public Quad {
 public:
  int index;
  int kind;
  CopyTAC(int i, int k) : index(i), kind(k) {}
  void print() const {
    std::cout << "this = " << index << '\n';
  }
};

// x relop y
  // jne L
class CondJumpRelopTAC : public Quad {
 public:
  char op;
  int lhs, rhs;
  int label;
  int kind;
  CondJumpRelopTAC(int l, int r, int lbl, int k)
      : lhs(l), rhs(r), label(lbl), kind(k) {}
  void print() const {
  }
};
// if x goto L
class CondJumpTAC : public Quad {
 public:
  int kind;
  int ident_index;
  int jmp_label;
  CondJumpTAC(int k, int idx, int label = 0)
      : kind(k), ident_index(idx), jmp_label(label) {}


       void print() const {
    std::cout << "if " << ident_index << " goto L" << jmp_label << std::endl;
  }
};

// jmp L
class JumpTAC : public Quad {
 public:
  int label;
  JumpTAC(int l = 0) : label(l) {}
};

// arg 1
// arg 2
// call f 2
class CallTAC : public Quad {
 public:
  void * func;
  int num_args;
  std::string fun;
  CallTAC(void * f, int n) : func(f), num_args(n) {}

  void print() const {
    std::cout << "call " << fun << '\n';
  }

};

// arg x
class ArgTAC : public Quad {
 public:
  int arg;
  int kind;
  ArgTAC(int a, int k) : arg(a), kind(k) {}

    void print() const { std::cout << "arg " << arg << std::endl; }
};

// x = y[i]
class IndexCopyTAC : public Quad {
 public:
  const Symbol* arr;
  int index;
  int kind;
  IndexCopyTAC(const Symbol* a, int i, int k) : arr(a), index(i), kind(k) {}

   void print() const {
    std::cout <<  " = " << arr->name << "[" << index << "]"
              << std::endl;
  }
};

// x[i] = y
class ValueCopyTAC : public Quad {
 public:
  const Symbol* arr;
  int index;
  std::string value;
  int kind;
  ValueCopyTAC(const Symbol* a, int i, int k) : arr(a), index(i), kind(k) {}


    void print() const {
            std::cout << arr->name << "[" << index << "]" << " = " << value << std::endl;
        }
};

// x = &y
class AddrTac : public Quad {
 public:
  int index;
  int kind;
  AddrTac(int i, int k) : index(i), kind(k) {}


   void print() const { std::cout << " = &" << index << std::endl; }
};

// x = *y
class DerefTAC : public Quad {
 public:
  int index;
  int kind;
  DerefTAC(int i, int k) : index(i), kind(k) {}

  void print() const { std::cout << " = *" << index << std::endl; }
};
// *x = y
class AddrSetTAC : public Quad {
 public:
  int index;
  int kind;
  int value;
  AddrSetTAC(int i, int k, int v) : index(i), kind(k), value(v) {}


    void print() const {
            std::cout << "*" << index << " = " << value << std::endl;
        }
};

class CastTAC : public Quad {
 public:
  int index;
  int source_kind;
  int destination_kind;
  CastTAC(int i, int src, int dest)
      : index(i), source_kind(src), destination_kind(dest) {}


       void print() const { std::cout << "cast " << index << std::endl; }
};

// mov(suffix) %eax
// ret
class ReturnTAC : public Quad {
 public:
  int ret_val;
  int kind;

  ReturnTAC(int ret, int k) : ret_val(ret), kind(k) {}

  void print() const { std::cout << "ret " << ret_val << std::endl; }
};

class BasicBlock {
 public:
  std::vector<Quad*> trips = std::vector<Quad*>();

  std::string name;

  int n_instructions;

  void add_block(std::string name);

  void add_instruction(Quad* trip) {
    trips.push_back(trip);
    ++n_instructions;
  }

  int last_instruction() const { return n_instructions - 1; }

  int next_instruction() const { return n_instructions; }

  int get_instruction(const ASTNode & n);

  void print() const;
  BasicBlock* prev;
};

namespace x {
extern BasicBlock* bblock;
}

#endif
