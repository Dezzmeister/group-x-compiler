#include "tac.h"

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

void BasicBlock::print() const {
    std::cout << name << ":" << '\n';
    for (auto trip : trips) {
        trip->print();
    }
    std::cout << "ret" << '\n';
    if (this->prev) {
        this->prev->print();
    }
}

