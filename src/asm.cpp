#include "asm.h"

RegisterState::RegisterState() : var("") {}

AsmState::AsmState() : stack({}), offsets({}), regs{}, stack_offset(0), reg_roulette(0) {}

std::optional<VarLoc> AsmState::find_var(std::string id) {
    for (int i = 0; i < GeneralReg::Count; i++) {
        if (regs[i].used && regs[i].var == id) {
            VarLoc loc = {
                .loc_type = Reg,
                .loc = { .reg = (GeneralReg) i }
            };

            return std::optional<VarLoc>(loc);
        }
    }

    int offset = 0;

    for (auto &item : stack) {
        if (item.id == id) {
            StackVarLoc loc = {
                .offset = offset,
                .var = &item
            };

            VarLoc var_loc = {
                .loc_type = Stack,
                .loc = { .stack = loc }
            };

            return std::optional<VarLoc>(var_loc);
        }

        offset += item.size;
    }

    return std::nullopt;
}

std::optional<GeneralReg> AsmState::get_unused_reg() {
    for (int i = 0; i < GeneralReg::Count; i++) {
        if (!regs[i].used) {
            return std::optional<GeneralReg>((GeneralReg) i);
        }
    }

    return std::nullopt;
}

GeneralReg AsmState::free_reg(std::ostream &code) {
    std::optional<GeneralReg> reg = get_unused_reg();

    if (reg) {
        return *reg;
    }

    RegisterState * state = regs + reg_roulette;

    stack_offset += 8;
    code << "push %" << REG_NAMES[reg_roulette] << "# save register on stack, variable: " << state->var << "\n";
    state->used = false;
    StackVar stack_var = {
        .size = 8,
        .id = regs[reg_roulette].var
    };
    stack.push_back(stack_var);

    reg_roulette = (reg_roulette + 1) % GeneralReg::Count;

    return (GeneralReg) (reg_roulette - 1);
}

GeneralReg AsmState::move_into_reg(std::string id, std::ostream &code, int offset) {
    std::optional<VarLoc> var_loc_opt = find_var(id);

    if (!var_loc_opt) {
        fprintf(stderr, "variable not in register file or stack\n");
        exit(1);
    }

    VarLoc loc = *var_loc_opt;

    // var is already in reg, return reg
    if (loc.loc_type == Reg) {
        return loc.loc.reg;
    }

    StackVarLoc stack_loc = loc.loc.stack;

    int reg = -1;

    for (size_t i = offset; i < GeneralReg::Count; i++) {
        if (!regs[i].used) {
            reg = i;
        }
    }    

    if (reg == -1) {
        reg_roulette = offset;
        this->free_reg(code);
    }

    // mov -offset(%rsp), %reg
    code << "movq -" << stack_loc.offset << "(%rsp), %" << REG_NAMES[reg] << "\n";
    regs[reg].used = true;
    regs[reg].var = id;

    return (GeneralReg) reg;
}

void AsmState::clear_var(std::string id) {
    for (size_t i = 0; i < GeneralReg::Count; i++) {
        if (regs[i].used && regs[i].var == id) {
            regs[i].used = false;
            return;
        }
    }
}

void AsmState::clear_reg(GeneralReg reg, std::ostream &code) {
    if (!regs[reg].used) {
        return;
    }

    code << "push %" << REG_NAMES[reg] << "# force clear register: " << regs[reg].var << "\n";
    regs[reg].used = false;
    stack_offset += 8;
    StackVar stack_var = {
        .size = 8,
        .id = regs[reg].var
    };
    stack.push_back(stack_var);
}

void AsmState::new_frame() {
    offsets.push(stack_offset);
    stack_offset = 0;
}

int AsmState::pop_frame() {
    int out = stack_offset;
    int size = 0;
    while (size != stack_offset) {
        StackVar var = stack[stack.size() - 1];
        size += var.size;
        stack.pop_back();
    }

    stack_offset = offsets.top();
    offsets.pop();

    return out;
}

void AsmState::reserve_space(std::ostream &code, std::string name, int size) {
    stack_offset += size;
    stack.push_back({
        .size = size,
        .id = name
    });

    code << "subq $" << size << ", %rsp\n";
}

void AsmState::copy_addr(std::ostream &code, GeneralReg src_addr, GeneralReg dest_addr, int size) {
    for (int i = 0; i < size; i+=8) {
        code << "movq " << i << "(%" << REG_NAMES[src_addr] << "), " << i << "(%" << REG_NAMES[dest_addr] << ")\n";
    }
}
