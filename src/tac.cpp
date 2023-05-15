#include "tac.h"
#include "asm_utils.h"

#include <iostream>
#include <iomanip>

BasicBlock *x::bblock = nullptr;

std::string next_l() {
    static int i = 0;
    return ".L" + std::to_string(i++);
}

std::string next_t() {
    static int i = 0;
    return "_t" + std::to_string(i++);
}

std::string next_p() {
    static int i = 0;
    return "_p" + std::to_string(i++);
}

template<>
void Value<bool>::print() const {
    std::cout << id << " = " << std::boolalpha << value;
}

template <>
void Value<char>::print() const {
    std::cout << id << " = ";
    switch (value) {
        case '\n':
            printf("\\n");
            break;

        case '\t':
            printf("\\t");
            break;

        case '\0':
            printf("\\0");
            break;

        case '\e':
            printf("\\e");
            break;

        case '\r':
            printf("\\r");
            break;

        default:
            printf("'%c'", value);
    }
}

void BasicBlock::add_block(std::string name)
{
    BasicBlock *new_block = new BasicBlock();
    new_block->name = name;
    new_block->prev = this;
    x::bblock = new_block;
}

void Quad::print() const {}

void CallTAC::print() const {
    std::cout << "call " << fun;
}

void RetvalTAC::print() const {
    std::cout << id << " = __retval";
}

void ArgTAC::print() const {
    std::cout << id << " = __arg(" << arg << ")";
}

void VoidReturnTAC::print() const {
    std::cout << "ret";
}

void ReturnTAC::print() const {
    std::cout << "ret " << id;
}

void CmpLiteralTAC::print() const {
    std::cout << "cmp " << id << ", " << literal;
}

void JneTAC::print() const {
    std::cout << "jne " << label;
}

void LogicalTAC::print() const {
    std::cout << id << " = " << left << " " << op << " " << right;
}

void MathTAC::print() const {
    std::cout << id << " = " << left << " " << op << " " << right;
}

void AssignTAC::print() const
{
    std::cout << id << " = " << rhs;
}

void DeleteTAC::print() const {
    std::cout << "del " << id;
}

void LabelTAC::print() const {
    std::cout << label << ":";
}

void SetupStackTAC::print() const {
    std::cout << "setup_stack";
}

void BasicBlock::print() const
{
    std::cout << name << ":" << '\n';

    int i = 0;
    for (auto trip : trips)
    {
        std::cout << i++ << ": ";
        trip->print();
        std::cout << '\n';
    }
    std::cout << i++  << ": ret" << '\n' << '\n';
    if (this->prev)
    {
        this->prev->print();
    }
}

std::string CharTAC(const char c, std::vector<Quad *> instrs) {
    std::string temp = next_t();
    Value<char> *value = new Value<char>(temp, c);
    instrs.push_back(value);
    return temp;
}

std::string FloatLitTAC(const float val, std::vector<Quad *> instrs, SymbolTable * global_symtable)
{
    std::string temp = next_t() ;
    std::cout << temp << " = " << val ;
    Value<float> *value = new Value<float>(temp, val);
    instrs.push_back(value) ;
    // wait for joe symbol table thing
    // global_symtable->put(temp, val) ;
    return temp ;
}

std::string IntLitTAC(const int val, std::vector<Quad *> instrs, SymbolTable * global_symtable)
{
    std::string temp = next_t() ;
    std::cout << temp << " = " << val;
    Value<int> *value = new Value<int>(temp, val);
    instrs.push_back(value) ;
    // wait for joe symbol table thing
    // global_symtable->put(temp, val) ;
    return temp ;
    
}

std::string BoolLitTAC(const bool b, std::vector<Quad *> instrs, SymbolTable * global_symtable)
{
    std::string temp = next_t() ;
    std::cout << temp << " = " << b;
    Value<bool> *value = new Value<bool>(temp, b);
    instrs.push_back(value) ;
    // wait for joe symbol table thing
    // global_symtable->put(temp, val) ;
    return temp ;
}

std::string StringLitTAC(const std::string s, std::vector<Quad *> instrs, SymbolTable * global_symtable)
{
    std::string temp = next_t() ;
    std::cout << temp << " = " << s;
    Value<std::string> *value = new Value<std::string>(temp, s);
    instrs.push_back(value) ;
    // wait for joe symbol table thing
    // global_symtable->put(temp, val) ;
    return temp;
}

template <>
void Value<int>::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    const GeneralReg reg = state.free_reg(code);
    state.regs[reg].used = true;
    state.regs[reg].var = id;
    code << "movq $" << value << ", %" << REG_NAMES[reg] << "\n";
}

template<>
void Value<float>::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    // TODO: fill in
}

template<>
void Value<bool>::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    // TODO: fill in
}

template<>
void Value<char>::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    // TODO: fill in
}

template<>
void Value<std::string>::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    // TODO: fill in
}

void AssignTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    std::optional<VarLoc> id_loc = state.find_var(id);
    GeneralReg rhs_reg = state.move_into_reg(rhs, code, 0);

    if (!id_loc) {
        GeneralReg reg = state.free_reg(code);
        state.regs[reg].used = true;
        state.regs[reg].var = id;
        code << "movq %" << REG_NAMES[rhs_reg] << ", %" << REG_NAMES[reg] << "\n";
        return;
    }

    VarLoc loc = *id_loc;

    if (loc.loc_type == Reg) {
        GeneralReg reg = loc.loc.reg;
        state.regs[reg].used = true;
        state.regs[reg].var = id;
        code << "movq %" << REG_NAMES[rhs_reg] << ", %" << REG_NAMES[reg] << "\n";
        return;
    }

    StackVarLoc stack_loc = loc.loc.stack;
    code << "movq %" << REG_NAMES[rhs_reg] << ", -" << stack_loc.offset << "(%rsp)\n";
}

void DeleteTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    state.clear_var(id);
}

void CmpLiteralTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    GeneralReg reg = state.move_into_reg(id, code, 0);

    code << "cmp %" << REG_NAMES[reg] << ", $" << literal << "\n";
}

void JneTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    code << "jne " << label << "\n";
}

void LogicalTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    state.clear_reg(GeneralReg::Rax, code);
    state.regs[GeneralReg::Rax].used = true;
    state.regs[GeneralReg::Rax].var = id;
    
    GeneralReg lhs = state.move_into_reg(left, code, GeneralReg::Rax + 1);
    GeneralReg rhs = state.move_into_reg(right, code, GeneralReg::Rax + 1);
    std::string true_label = next_l();
    std::string false_label = next_l();

    if (op == "==") {
        code << "cmp %" << REG_NAMES[lhs] << ", %" << REG_NAMES[rhs] << "\n";
        code << "jne " << false_label << "\n";
        code << "movq %rax, $1\n";
        code << false_label << ":\n";
        code << "movq %rax, $0\n";
        code << true_label << ":\n";
    } else if (op == "!=") {
        code << "cmp %" << REG_NAMES[lhs] << ", %" << REG_NAMES[rhs] << "\n";
        code << "je " << false_label << "\n";
        code << "movq %rax, $1\n";
        code << false_label << ":\n";
        code << "movq %rax, $0\n";
        code << true_label << ":\n";
    } else if (op == ">") {
        code << "cmp %" << REG_NAMES[lhs] << ", %" << REG_NAMES[rhs] << "\n";
        code << "jle " << false_label << "\n";
        code << "movq %rax, $1\n";
        code << false_label << ":\n";
        code << "movq %rax, $0\n";
        code << true_label << ":\n";
    } else if (op == "<") {
        code << "cmp %" << REG_NAMES[lhs] << ", %" << REG_NAMES[rhs] << "\n";
        code << "jge " << false_label << "\n";
        code << "movq %rax, $1\n";
        code << false_label << ":\n";
        code << "movq %rax, $0\n";
        code << true_label << ":\n";
    } else if (op == ">=") {
        code << "cmp %" << REG_NAMES[lhs] << ", %" << REG_NAMES[rhs] << "\n";
        code << "jl " << false_label << "\n";
        code << "movq %rax, $1\n";
        code << false_label << ":\n";
        code << "movq %rax, $0\n";
        code << true_label << ":\n";
    } else if (op == "<=") {
        code << "cmp %" << REG_NAMES[lhs] << ", %" << REG_NAMES[rhs] << "\n";
        code << "jg " << false_label << "\n";
        code << "movq %rax, $1\n";
        code << false_label << ":\n";
        code << "movq %rax, $0\n";
        code << true_label << ":\n";
    } else if (op == "in") {
        fprintf(stderr, "in kw not supported\n");
        exit(1);
    } else if (op == "not in") {
        fprintf(stderr, "not in kw not supported\n");
        exit(1);
    }
}

void MathTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    state.clear_reg(GeneralReg::Rax, code);
    state.regs[GeneralReg::Rax].used = true;
    state.regs[GeneralReg::Rax].var = id;
    GeneralReg lhs = state.move_into_reg(left, code, GeneralReg::Rax + 1);
    GeneralReg rhs = state.move_into_reg(right, code, GeneralReg::Rax + 1);
    std::string true_label = next_l();
    std::string false_label = next_l();

    if (op == '+') {
        code << "movq %" << REG_NAMES[rhs] << ", %rax\n";
        code << "addq %" << REG_NAMES[lhs] << ", %rax\n";
    } else if (op == '-') {
        code << "movq %" << REG_NAMES[rhs] << ", %rax\n";
        code << "subq %" << REG_NAMES[lhs] << ", %rax\n";
    } else if (op == '*') {
        code << "movq %" << REG_NAMES[rhs] << ", %rax\n";
        code << "imul %rax, %" << REG_NAMES[lhs] << "\n";
    } else if (op == '/') {
        code << "movq %" << REG_NAMES[lhs] << ", %rax\n";
        code << "idiv %" << REG_NAMES[rhs] << "\n";
    } else if (op == '%') {
        code << "movq %" << REG_NAMES[lhs] << ", %rax\n";
        code << "idiv %" << REG_NAMES[rhs] << "\n";
        code << "movq %rdx, %rax\n";
    }
}

void LabelTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    code << label << ":\n";
}

void PushTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    GeneralReg reg = state.move_into_reg(id, code, 0);
    code << "pushq %" << REG_NAMES[reg] << "\n";
    state.stack_offset += 8;
    state.regs[reg].used = false;
    StackVar stack_var = {
        .size = 8,
        .id = state.regs[reg].var
    };
    state.stack.push_back(stack_var);
}

void SetupStackTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    code << "pushq %rbp\n";
    code << "movq %rsp, %rbp\n";
}

void CallTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    code << "call " << fun << "\n";
    state.pop_frame();
    code << "movq %rbp, %rsp\n";
}

void RetvalTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    GeneralReg reg = state.move_into_reg(id, code, 0);

    if (reg == GeneralReg::Rax) {
        return;
    }

    code << "movq %" << REG_NAMES[reg] << ", %rax #retval\n";
    state.regs[GeneralReg::Rax].used = true;
    state.regs[GeneralReg::Rax].var = "<retval>";
}

void ArgTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    GeneralReg reg = state.free_reg(code);
    int offset = type_table->arg_offset(this);
    code << "movq -" << offset << "(%rsp), %" << REG_NAMES[reg] << " # arg: " << arg << "\n";
    state.regs[reg].used = true;
    state.regs[reg].var = id;
}

void VoidReturnTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    code << "leave\n";
    code << "ret\n";
}

void ReturnTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    state.clear_reg(GeneralReg::Rax, code);
    GeneralReg reg = state.move_into_reg(id, code, GeneralReg::Rax + 1);

    code << "movq %" << REG_NAMES[reg] << ", %rax # prepare return value\n";
    code << "leave\n";
    code << "ret\n";
    state.regs[GeneralReg::Rax].used = true;
    state.regs[GeneralReg::Rax].var = id;
}
