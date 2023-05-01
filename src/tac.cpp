#include "tac.h"

BasicBlock * x::bblock = new BasicBlock(); 

void BasicBlock::add_block(std::string name) {
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

