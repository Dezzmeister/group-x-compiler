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
    return "__t" + std::to_string(i++);
}

std::string next_p() {
    static int i = 0;
    return "__p" + std::to_string(i++);
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

void LabelTAC::print() const {
    std::cout << label << ":";
}

void JumpTAC::print() const {
    std::cout << "jmp " << label;
}

void CallTAC::print() const {
    std::cout << "call " << fun;
}

void RetvalTAC::print() const {
    std::cout << id << " = __retval";
}

void CopyTAC::print() const
{
    std::cout << "copy " << index;
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

void LoadTAC::print() const
{
    std::cout << identifier;
}

void AssignTAC::print() const
{
    std::cout << lhs << " " << op << " " << rhs;
}

void MoveTAC::print() const
{
    std::cout << location << " = " << index;
}

void CondJumpTAC::print() const {
    std:: cout << "if " << ident_index << " jmp " << jmp_label;
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
