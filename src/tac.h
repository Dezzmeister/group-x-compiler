#ifndef TAC_H
#define TAC_H

#include <vector>
#include <cstdint>


// Abstract TAC class.
class Triple {
};

// x = y op z
class AssignTAC : public Triple {
    public:
};

// x = y
class CopyTAC : public Triple {
    public:
};

// x = y
// jne L
class CondJumpTAC : public Triple {
    public:
};

// jmp L
class JumpTac : public Triple {
    public:
};

// param x1
// param x2
// call f 2  /* call function f with two parameters */
class FunTAC : public Triple {
    public:
};

// t = y[i]
class IndexTAC : public Triple {
    public:
};

// x = t
// Same as CopyTAC, but for arrays.
// Should we get rid of this?
class ArrayTac : public Triple {
    public:
};

// x = &y
class AddrTac : public Triple {
    public:
};

// x = *y
class PtrTac : public Triple {
    public:
};

class BasicBlock {
    // See Aho Section 8.4
    public:
    std::vector<Triple> trips;
    int num_instr;
    BasicBlock(std::vector<Triple> t) : trips(t) {}
    BasicBlock() {
        num_instr = 0;
        trips = std::vector<Triple>();
    }

    int add_trip(Triple t) {
        trips.push_back(t);
        return num_instr++;
    }
};

namespace x {
    extern BasicBlock * block;
}

#endif
