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
