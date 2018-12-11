#ifndef OPCODE_HPP
#define OPCODE_HPP

enum OpCode {
    OP_LA, OP_LV, OP_LC, OP_LI,

    OP_INC, OP_DEC,

    OP_JMP, OP_FJMP,

    OP_HALT, OP_STORE, OP_CALL, OP_RET,

    OP_RI, // Read Int
    OP_WI, // Write Int
    OP_WL,  // Write Line

    OP_ADD, OP_SUB, 
    OP_MUL, OP_DIV, OP_MOD,
    OP_NEG, 

    OP_CV,

    OP_EQ, OP_NE, OP_GT, OP_LT, OP_GE, OP_LE,
};

struct Instruction {
    enum OpCode op;
    int p, q;
};

#endif
