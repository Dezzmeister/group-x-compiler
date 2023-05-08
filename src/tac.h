#ifndef SRC_TAC_H
#define SRC_TAC_H

#include <cstdint>

#include "ast.h"
#include "symtable.h"

class Quad {
 public:
  virtual void print() const = 0;
};

template <typename T>
// x = y op z
class AssignTAC : public Quad {
 public:
  std::string op;
  T addr1;
  T addr2;
  std::string result;
  int kind;
  AssignTAC(std::string o, T a1, T a2, int k)
      : op(o), addr1(a1), addr2(a2), kind(k) {}

  void print() const {
    std::cout << result << " = " << addr1 << " " << op << " " << addr2
              << std::endl;
  }
};

class UnaryTAC : public Quad {
 public:
  std::string op;
  int index;
  std::string result;
  int kind;

  UnaryTAC(std::string o, int i, int k) : op(o), index(i), kind(k) {}

  void print() const {
    std::cout << result << " = " << op << index << std::endl;
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

  void print() const { std::cout << "jmp L" << label << std::endl; }
};

// arg 1
// arg 2
// call f 2
class CallTAC : public Quad {
 public:
  std::string fun;
  int num_args;
  CallTAC(std::string f, int n) : fun(f), num_args(n) {}

  void print() const {
    std::cout << "call " << fun << " " << num_args << std::endl;
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
  std::string result;
  int kind;
  IndexCopyTAC(const Symbol* a, int i, int k) : arr(a), index(i), kind(k) {}

  void print() const {
    std::cout << result << " = " << arr->name << "[" << index << "]"
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
  std::string result;
  int kind;
  AddrTac(int i, int k) : index(i), kind(k) {}

  void print() const { std::cout << result << " = &" << index << std::endl; }
};

// x = *y
class DerefTAC : public Quad {
 public:
  int index;
  std::string result;
  int kind;
  DerefTAC(int i, int k) : index(i), kind(k) {}

  void print() const { std::cout << result << " = *" << index << std::endl; }
};

// *x = y
class AddrSetTAC: public Quad {
    public:
        int addr;
        std::string value;
        int kind;
        AddrSetTAC(int i, int k) : index(i), kind(k) {}

        void print() const {
            std::cout << "*" << addr << " = " << value << std::endl;
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
  std::string result;
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
