#include "asm.h"

AsmState::AsmState() : stack(std::vector<StackVar>()), regs{ {0} }, stack_offset(0), reg_roulette(0) {}

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

    reg_roulette = (reg_roulette + 1) % GeneralReg::Count;

    return (GeneralReg) (reg_roulette - 1);
}

GeneralReg AsmState::move_into_reg(std::string id, std::ostream &code) {
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
    
    GeneralReg reg = this->free_reg(code);

    // mov -offset(%rsp), %reg
    code << "movq -" << stack_loc.offset << "(%rsp), %" << REG_NAMES[reg] << "\n";
    regs[reg].used = true;
    regs[reg].var = id;

    return reg;
}

void AsmState::clear_var(std::string id) {
    for (size_t i = 0; i < GeneralReg::Count; i++) {
        if (regs[i].used && regs[i].var == id) {
            regs[i].used = false;
            return;
        }
    }
}
