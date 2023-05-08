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
// x = y op z
class AssignTAC : public Quad {
 public:
  std::string op;
  T addr1;
  T addr2;
  int kind;
  AssignTAC(std::string o, T a1, T a2, int k)
      : op(o), addr1(a1), addr2(a2), kind(k) {}
      
  void print() const {
    std::cout << " = " << addr1 << " " << op << " " << addr2
              << std::endl;
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
  const Symbol* symbol;
  int index;
  std::string target;
  int kind;
  CopyTAC(const Symbol* s, int k) : symbol(s), kind(k) {}
  CopyTAC(int i, int k) : index(i), kind(k) {}

   void print() const {
    std::cout << target << " = " << symbol->name << std::endl;
  }
};

template <typename T>
// x relop y
// jne L
class CondJumpRelopTAC : public Quad {
 public:
  char op;
  T addr1;
  T addr2;
  int label;
  int kind;
  CondJumpRelopTAC(T a1, T a2, int l, int k)
      : addr1(a1), addr2(a2), label(l), kind(k) {}

      void print() const {
    std::cout << addr1 << " " << op << " " << addr2 << std::endl;
    std::cout << "jne L" << label << std::endl;
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

template <typename T>
// arg x
class ArgTAC : public Quad {
 public:
  int kind;
  T arg;
  ArgTAC(T a, int k) : arg(a), kind(k) {}

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
template <typename T>
class AddrSetTAC : public Quad {
 public:
  int index;
  int kind;
  T value;
  AddrSetTAC(int i, int k, T v) : index(i), kind(k), value(v) {}


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
template <typename T>
class ReturnTAC : public Quad {
 public:
  T ret_val;
  int kind;
  ReturnTAC(T ret, int k) : ret_val(ret), kind(k) {}


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

  void print() const;
  BasicBlock* prev;
};

namespace x {
extern BasicBlock* bblock;
}

#endif
