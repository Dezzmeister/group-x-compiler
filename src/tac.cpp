#include "tac.h"

BasicBlock* x::block = new BasicBlock();

int BasicBlock::add_instr(Triple trip) {
  x::block->trips.push_back(trip);
  return x::block->num_instr++;
}

int BasicBlock::new_label() { return x::block->num_instr++; }