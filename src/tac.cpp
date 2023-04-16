#include "tac.h"

BasicBlock * x::block = new BasicBlock();

int Triple::make_triple(int op, intptr_t a1, intptr_t a2) {
        x::block->trips.push_back(Triple(op, a1, a2));
        return x::block->num_instructions++;
}