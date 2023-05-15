#include "tac.h"

#include <iostream>
#include <iomanip>

BasicBlock *x::bblock = nullptr;

std::string next_t() {
    static int i = 0;
    return "t"+ std::to_string(i++);
}

template<>
void Value<bool>::print() const {
    std::cout << next_t() << " = " << std::boolalpha << value;
}

template <>
void Value<char>::print() const {
    std::cout << next_t() << " = ";
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

void JumpTAC::print() const {
    std::cout << "jmp " << label;
}

void CallTAC::print() const {
    std::cout << "call " << fun << ' ' << num_args;
}

void CopyTAC::print() const
{
    std::cout << "copy " << index;
}

void LoadTAC::print() const
{
    std::cout << identifier;
}

void AssignTAC::print() const
{
    std::cout << lhs << ' ' << op << ' ' << rhs;
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

std::string CharTAC(const char c) {
    std::string temp = next_t();
    instrs.pushback(temp + " = '" + std::string(1, c) + "'");
    return temp;
}
