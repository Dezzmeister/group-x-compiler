#ifndef SRC_ASM_H
#define SRC_ASM_H

#include <stack>

// Registers for general purpose use
enum GeneralRegs {
    Rax,
    Rbx,
    Rcx,
    Rdx,
    Rsi,
    Rdi,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    Count
};

const char * const REG_NAMES[] = {
    "rax",
    "rbx",
    "rcx",
    "rdx",
    "rsi",
    "rdi",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15"
};

struct RegisterState {
    // int representing the variable in this register
    int var;
    // If true, this register is in use and cannot be cleaned up
    bool used;
};

struct AsmState {
    std::stack<int> stack;
    int regs[GeneralRegs::Count];

    AsmState();
};

#endif