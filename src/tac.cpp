#include "tac.h"

#include <iostream>

BasicBlock * x::bblock = new BasicBlock(); 

int BasicBlock::get_instruction(const ASTNode & n) {
    n.gen_tac();
    return this->last_instruction();
}

void BasicBlock::add_block(std::string name) {
    BasicBlock * new_block = new BasicBlock();
    new_block->name = name;
    new_block->prev = this;
    x::bblock = new_block;
}

void Quad::print() const {}

void CopyTAC::print() const {
    std::cout <<  "copy " << index;
}

void LoadTAC::print() const {
    std::cout << identifier;
}

void AssignTAC::print() const {
    std::cout << lhs << ' ' << op << ' ' << rhs; 
}

void BasicBlock::print() const {
    std::cout << name << ":" << '\n';
    int i = 0;
    for (auto trip : trips) {
        std::cout << i++ << ": ";
        trip->print();
        std::cout << '\n';
    }
    std::cout << "ret" << '\n';
    if (this->prev) {
        this->prev->print();
    }
}
