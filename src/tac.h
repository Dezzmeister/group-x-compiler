#ifndef TAC_H
#define TAC_H

#include <vector>
#include <cstdint>

// three address code class.

class Triple {
    public:
    int op;
    intptr_t addr1;
    intptr_t addr2;
    Triple(int op, intptr_t a1, intptr_t a2) :
    op(op), addr1(a1), addr2(a2) {}
    static int make_triple(int op, intptr_t a1, intptr_t a2);
};

class BasicBlock {
    // See Aho Section 8.4
    public:
    std::vector<Triple> trips;
    int num_instructions;
    BasicBlock(std::vector<Triple> t) : trips(t) {}
    BasicBlock() {
        num_instructions = 0;
        trips = std::vector<Triple>();
    }
};

namespace x {
    extern BasicBlock * block;
}

#endif
