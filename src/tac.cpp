#include "tac.h"

BasicBlock * x::block = new BasicBlock(std::vector<Quad>());

int BasicBlock::add_instr(Quad trip) {
    x::block->trips.push_back(trip);
    return x::block->n_instr++;
}
