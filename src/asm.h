#ifndef SRC_ASM_H
#define SRC_ASM_H

#include <iostream>
#include <stack>
#include <optional>
#include <vector>

#define NELEM(a) (sizeof(a) / sizeof(*a))

// Registers for general purpose use
enum GeneralReg {
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

    // Constexpr representing the number of registers, NOT A REGISTER
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

static_assert(GeneralReg::Count == NELEM(REG_NAMES));

typedef struct {
    int size;
    std::string id;
} StackVar;

typedef enum {
    Stack,
    Reg
} VarLocType;

// Does not own var
typedef struct {
    int offset;
    StackVar * var;
} StackVarLoc;

typedef union {
    GeneralReg reg;
    StackVarLoc stack;
} VarLocInfo;

typedef struct {
    VarLocType loc_type;
    VarLocInfo loc;
} VarLoc;

typedef struct RegisterState {
    // id of variable in this register
    std::string var;
    // If true, this register is in use and cannot be cleaned up
    bool used;

    RegisterState();

} RegisterState;

typedef struct AsmState {
    std::vector<StackVar> stack;
    std::stack<int> offsets;
    RegisterState regs[GeneralReg::Count];
    int stack_offset;
    // Index of next register to move if all registers are in use
    // and we need one
    int reg_roulette;

    AsmState();

    /**
     * Looks in the register file first. Then looks in the stack
     */
    std::optional<VarLoc> find_var(std::string id);

    std::optional<GeneralReg> get_unused_reg();

    /**
     * Frees a register by moving the variable to the stack. Updates the stack offset
     * and returns the free register. If a free register already exists, no asm is emitted.
     * The free register is returned
     */
    GeneralReg free_reg(std::ostream &code);

    GeneralReg move_into_reg(std::string id, std::ostream &code, int offset);

    void clear_var(std::string id);

    void clear_reg(GeneralReg reg, std::ostream &code);

    void new_frame();

    int pop_frame();

} AsmState;

#endif