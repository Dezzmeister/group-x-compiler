#include "tac.h"

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
    state.regs[reg].var = "<literal>";
    code << "movq %" << REG_NAMES[reg] << ", $" << value << "\n";
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
    GeneralReg rhs_reg = state.move_into_reg(rhs, code);

    if (!id_loc) {
        GeneralReg reg = state.free_reg(code);
        state.regs[reg].used = true;
        state.regs[reg].var = id;
        code << "movq %" << REG_NAMES[reg] << ", %" << REG_NAMES[rhs_reg] << "\n";
        return;
    }

    VarLoc loc = *id_loc;

    if (loc.loc_type == Reg) {
        GeneralReg reg = loc.loc.reg;
        state.regs[reg].used = true;
        state.regs[reg].var = id;
        code << "movq %" << REG_NAMES[reg] << ", %" << REG_NAMES[rhs_reg] << "\n";
        return;
    }

    StackVarLoc stack_loc = loc.loc.stack;
    code << "movq -" << stack_loc.offset << "(%rsp), %" << REG_NAMES[rhs_reg] << "\n";
}

void DeleteTAC::to_asm(std::ostream &code, TypeTable * type_table, NamesToNames &names, AsmState &state) const {
    state.clear_var(id);
}
