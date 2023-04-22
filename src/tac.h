#ifndef TAC_H
#define TAC_H

#include <cstdint>
#include <vector>

#include "ast.h"
#include "symtable.h"

// Abstract TAC class.
class Triple {};

template <typename T, typename U>
// x = y op z
class AssignTAC : public Triple {
 public:
  char op;
  T addr1;
  U addr2;
  AssignTAC(char o, T a1, U a2) : op(o), addr1(a1), addr2(a2) {}
};

// x = y
class CopyTAC : public Triple {
 public:
  Symbol* symbol;
};

// x - y
// jne L
template <typename T, typename U>
class CondJumpRelopTAC : public Triple {
 public:
  char op;
  T addr1;
  U addr2;
  int label;
};

class CondJumpTAC : public Triple {
 public:
  Symbol* addr;
  int index;
  int label;
  CondJumpTAC(int i, int l) : index(i), label(l) {}
  CondJumpTAC(Symbol* s, int l) : addr(s), label(l) {}
};

class LoadTAC : public Triple {
 public:
  Symbol* addr;
  LoadTAC(Symbol* s) : addr(s) {}
};

// jmp L
class JumpTac : public Triple {
 public:
  int label;
};

// param x1
// param x2
// call f 2  /* call function f with two parameters */
class CallTAC : public Triple {
 public:
  Symbol* fun;
  int num_args;
  CallTAC(Symbol* f, int n) : fun(f), num_args(n) {}
};

template <typename T>
class ParamTAC : public Triple {
 public:
  T param;
};

// t = y[i]
class IndexTAC : public Triple {
 public:
  int index;
  Symbol* arr;
};

// x = &y
class AddrTac : public Triple {
 public:
  int index;
  AddrTac(int i) : index(i) {}
};

// x = *y
class DerefTAC : public Triple {
 public:
  int index;
  DerefTAC(int i) : index(i) {}
};

class CastTAC : public Triple {
 public:
  const Typename* type;
  int index;
  CastTAC(const Typename* t, int i) : type(t), index(i) {}
};

class ReturnTAC : public Triple {
 public:
  int index;
  ReturnTAC(int i) : index(i) {}
};

class BasicBlock {
  // See Aho Section 8.4
 public:
  std::vector<Triple> trips;
  int num_instr;
  BasicBlock(std::vector<Triple> t) : trips(t) {}
  BasicBlock() {
    num_instr = 0;
    trips = std::vector<Triple>();
  }

  static int new_label();
  static int add_instr(Triple trip);
};

namespace x {
extern BasicBlock* block;
}

#endif
