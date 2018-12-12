#include "assembly.hpp"
#include <vector>

std::vector<Instruction> g_instructions;

void as_init() {
    g_instructions.clear();
    as_jmp();
}

void as_set_main(int addr) {
    g_instructions[0].p = addr;
}

void as_set_jmp_addr(int i, int addr) {
    g_instructions[i].p = addr;
}

int as_code_addr() {
    return g_instructions.size();
}
void as_set_main(int addr);

void as_save(FILE *file) {
    std::fwrite(
            g_instructions.data(), 
            sizeof(Instruction),
            g_instructions.size(),
            file);
}

void as_load_addr(int depth, int offset) {
    Instruction i;
    i.op = OP_LA;
    i.p = depth;
    i.q = offset;
    g_instructions.push_back(i);
}

void as_load_value(int depth, int offset) {
    Instruction i;
    i.op = OP_LV;
    i.p = depth;
    i.q = offset;
    g_instructions.push_back(i);
}

void as_load_const(int value) {
    Instruction i;
    i.op = OP_LC;
    i.p = value;
    g_instructions.push_back(i);
}

void as_load_indirect() {
    Instruction i;
    i.op = OP_LI;
    g_instructions.push_back(i);
}

void as_inc(int value) {
    Instruction i;
    i.op = OP_INC;
    i.p = value;
    g_instructions.push_back(i);
}

void as_dec(int value) {
    Instruction i;
    i.op = OP_DEC;
    i.p = value;
    g_instructions.push_back(i);
}

int as_jmp() {
    int index = g_instructions.size();
    Instruction i;
    i.op = OP_JMP;
    g_instructions.push_back(i);
    return index;
}

int as_false_jmp() {
    int index = g_instructions.size();
    Instruction i;
    i.op = OP_FJMP;
    g_instructions.push_back(i);
    return index;
}

void as_store() {
    Instruction i;
    i.op = OP_STORE;
    g_instructions.push_back(i);
}

void as_call(int depth, int addr) {
    Instruction i;
    i.op = OP_CALL;
    i.p = depth;
    i.q = addr;
    g_instructions.push_back(i);
}

void as_ret() {
    Instruction i;
    i.op = OP_RET;
    g_instructions.push_back(i);
}

void as_halt() {
    Instruction i;
    i.op = OP_HALT;
    g_instructions.push_back(i);
}

void as_read_int() {
    Instruction i;
    i.op = OP_RI;
    g_instructions.push_back(i);
}

void as_write_int() {
    Instruction i;
    i.op = OP_WI;
    g_instructions.push_back(i);
}

void as_add() {
    Instruction i;
    i.op = OP_ADD;
    g_instructions.push_back(i);
}

void as_sub() {
    Instruction i;
    i.op = OP_SUB;
    g_instructions.push_back(i);
}

void as_mul() {
    Instruction i;
    i.op = OP_MUL;
    g_instructions.push_back(i);
}

void as_div() {
    Instruction i;
    i.op = OP_DIV;
    g_instructions.push_back(i);
}

void as_mod() {
    Instruction i;
    i.op = OP_MOD;
    g_instructions.push_back(i);
}

void as_neg() {
    Instruction i;
    i.op = OP_NEG;
    g_instructions.push_back(i);
}

void as_odd() {
    Instruction i;
    i.op = OP_ODD;
    g_instructions.push_back(i);
}

void as_eq() {
    Instruction i;
    i.op = OP_EQ;
    g_instructions.push_back(i);
}

void as_gt() {
    Instruction i;
    i.op = OP_GT;
    g_instructions.push_back(i);
}

void as_ge() {
    Instruction i;
    i.op = OP_GE;
    g_instructions.push_back(i);
}

void as_lt() {
    Instruction i;
    i.op = OP_LT;
    g_instructions.push_back(i);
}

void as_le() {
    Instruction i;
    i.op = OP_LE;
    g_instructions.push_back(i);
}

void as_ne() {
    Instruction i;
    i.op = OP_NE;
    g_instructions.push_back(i);
}
