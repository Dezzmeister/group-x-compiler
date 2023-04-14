//three address code file

// A class that represents three address code.
class Quad  {
    /* 
    * Addresses, including result, can be either be addresses of constants 
    * or pointers to a symbol table entry. 
    */
    // E.g. t3 = t1 + t2 
    int op;
    int addr1;
    int addr2;
    // The result can be a symbol table entry or an temporary
    // used in another tac instruction.
    int result;
    Quad(int op, int a1, int a2, int res) :
    op(op), addr1(a1), addr2(a2), result(res) {}
};

class BasicBlock {
    // See Aho Section 8.4
    Quad * quads;
    BasicBlock(Quad * q) : quads(q) {}
};
