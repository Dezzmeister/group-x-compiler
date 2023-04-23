#ifndef TAC_H
#define TAC_H

#include <cstdint>
#include <vector>

#include "ast.h"
#include "symtable.h"

// Abstract TAC class.
class Quad {
  // Returns temporary names in a sequence like t1, t2, ...
  // These temporaries will be put into the symbol table.
  std::string next_temp() {
    static int counter = 1;
    return "t" + std::to_string(counter++);
}
};

template <typename T>
// x = y op z
class AssignTAC : public Quad {
 public:
  char op;
  T addr1;
  T addr2;
  std::string result;
  AssignTAC(char o, T a1, T a2) : op(o), addr1(a1), addr2(a2) {}
};

// x = y
class CopyTAC : public Quad {
 public:
  const Symbol* symbol;
  std::string target;
  CopyTAC(const Symbol* s) : symbol(s) {}
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
  CondJumpRelopTAC(T a1, T a2, int l) : addr1(a1), addr2(a2), label(l) {}
};

// if x goto L
class CondJumpTAC : public Quad {
 public:
  int index;
  int label;
  CondJumpTAC(int i, int l) : index(i), label(l) {}
};

// jmp L
class JumpTac : public Quad {
 public:
  int label;
  JumpTac(int l) : label(l) {}
};

// arg 1
// arg 2
// call f 2
class CallTAC : public Quad {
 public:
  Symbol* fun;
  int num_args;
  CallTAC(Symbol* f, int n) : fun(f), num_args(n) {}
};

template <typename T>
// arg x
class ArgTAC : public Quad {
 public:
  T arg;
  ArgTAC(T a) : arg(a) {}
};

// x = y[i]
class IndexCopyTAC : public Quad {
 public:
  const Symbol* arr;
  int index;
  std::string result;
  IndexCopyTAC(const Symbol* a, int i) : arr(a), index(i) {}
};

// x = &y
class AddrTac : public Quad {
 public:
  int index;
  std::string result;
  AddrTac(int i) : index(i) {}
};

// x = *y
class DerefTAC : public Quad {
 public:
  int index;
  std::string result;
  DerefTAC(int i) : index(i) {}
};

class CastTAC : public Quad {
 public:
  const Typename* type;
  int index;
  std::string result;
  CastTAC(const Typename* t, int i) : type(t), index(i) {}
};

class ReturnTAC : public Quad {
 public:
  int index;
  std::string result;
  ReturnTAC(int i) : index(i) {}
};

class BasicBlock {
 public:
  std::vector<Quad> trips;
  int num_instr;
  BasicBlock(std::vector<Quad> t) : trips(t) {}

  static int add_instr(Quad trip);
};

namespace x {
extern BasicBlock* block;
}

#endif
