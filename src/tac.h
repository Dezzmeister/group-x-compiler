#ifndef TAC_H
#define TAC_H

#include <cstdint>

#include "ast.h"
#include "symtable.h"

class Quad {
};

template <typename T>
// x = y op z
class AssignTAC : public Quad {
    public:
        char op;
        T addr1;
        T addr2;
        std::string result;
        int kind;
        AssignTAC(char o, T a1, T a2, int k) : op(o), addr1(a1), addr2(a2), kind(k) {}
};

// x = y
class CopyTAC : public Quad {
    public:
        const Symbol * symbol;
        std::string target;
        int kind;
        CopyTAC(const Symbol * s, int k) : symbol(s), kind(k) {}
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
};

// if x goto L
class CondJumpTAC : public Quad {
    public:
        int index;
        int label;
        int kind;
        CondJumpTAC(int i, int l, int k) : index(i), label(l), kind(k) {}
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
        Symbol * fun;
        int num_args;
        CallTAC(Symbol * f, int n) : fun(f), num_args(n) {}
};

template <typename T>
// arg x
class ArgTAC : public Quad {
    public:
        int kind;
        T arg;
        ArgTAC(T a, int k) : arg(a), kind(k) {}
};

// x = y[i]
class IndexCopyTAC : public Quad {
    public:
        const Symbol * arr;
        int index;
        std::string result;
        int kind;
        IndexCopyTAC(const Symbol * a, int i, int k) : arr(a), index(i), kind(k) {}
};

// x = &y
class AddrTac : public Quad {
    public:
        int index;
        std::string result;
        int kind;
        AddrTac(int i, int k) : index(i), kind(k) {}
};

// x = *y
class DerefTAC : public Quad {
    public:
        int index;
        std::string result;
        int kind;
        DerefTAC(int i, int k) : index(i), kind(k) {}
};


// mov(suffix) %eax
class ReturnTAC : public Quad {
    public:
        int index;
        std::string result;
        int kind;
        ReturnTAC(int i, int k) : index(i), kind(k) {}
};

class BasicBlock {
    public:
        std::vector<Quad> trips;
        int n_instr;
        BasicBlock(std::vector<Quad> t) : trips(t) {}
        static int add_instr(Quad trip);
};

namespace x {
    extern BasicBlock * block;
}

#endif
