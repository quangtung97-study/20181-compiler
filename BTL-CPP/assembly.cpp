#include "assembly.hpp"
#include <vector>

FILE *g_file;
std::vector<Instruction> g_instructions;

void as_init(FILE *file) {
    g_file = file;
    g_instructions.clear();
}

int as_pc() {
    return g_instructions.size();
}

void as_save() {
    std::fwrite(
            g_instructions.data(), 
            sizeof(Instruction),
            g_instructions.size(),
            g_file);
}

void as_load_const(int value) {
    Instruction i;
    i.op = OP_LC;
    i.p = value;
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

void as_halt() {
    Instruction i;
    i.op = OP_HALT;
    g_instructions.push_back(i);
}
