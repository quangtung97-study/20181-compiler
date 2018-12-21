#include "assembly.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

std::vector<Instruction> g_instructions;

void as_init() {
    g_instructions.clear();
    as_jmp();
}

const Instruction *as_instructions() {
    return g_instructions.data();
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

void as_write_line() {
    Instruction i;
    i.op = OP_WL;
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

static int digit_count(int num) {
    if (num == 0)
        return 1;

    int count = 0;
    while (num) {
        num /= 10;
        count++;
    }
    return count;
}

void as_print() {
    std::ostringstream ss;
    int padding = digit_count(g_instructions.size() - 1);

    auto print_line = [padding](int index) {
        std::cout << std::setw(padding) << index << "| ";
    };

    auto print_op = [](const char *op) {
        std::cout << std::setw(4) << std::left << op << std::right;
    };

    auto print_arg = [](int arg) {
        std::cout << std::setw(6) << arg;
    };

    auto print_single = [print_arg](Instruction i) {
        print_arg(i.p);
    };

    auto print_double = [print_arg](Instruction i) {
        print_arg(i.p);
        print_arg(i.q);
    };

    int index = 0;
    for (auto i: g_instructions) {
        print_line(index);
        switch (i.op) {
            case OP_LA:
                print_op("LA");
                print_double(i);
                break;

            case OP_LV:
                print_op("LV");
                print_double(i);
                break;

            case OP_LC:
                print_op("LC");
                print_single(i);
                break;

            case OP_LI:
                print_op("LI");
                break;

            case OP_INC:
                print_op("INC");
                print_single(i);
                break;

            case OP_DEC:
                print_op("DCT");
                print_single(i);
                break;

            case OP_JMP:
                print_op("J");
                print_single(i);
                break;

            case OP_FJMP:
                print_op("FJ");
                print_single(i);
                break;
                
            case OP_HALT:
                print_op("HLT");
                break;

            case OP_STORE:
                print_op("ST");
                break;

            case OP_CALL:
                print_op("CALL");
                print_double(i);
                break;

            case OP_RET:
                print_op("EP");
                break;

            case OP_RI:
                print_op("RI");
                break;

            case OP_WI:
                print_op("WI");
                break;

            case OP_WL:
                print_op("WL");
                break;

            case OP_ADD:
                print_op("ADD");
                break;

            case OP_SUB:
                print_op("SUB");
                break;

            case OP_MUL:
                print_op("MUL");
                break;

            case OP_DIV:
                print_op("DIV");
                break;

            case OP_MOD:
                print_op("MOD");
                break;

            case OP_NEG:
                print_op("NEG");
                break;

            case OP_EQ:
                print_op("EQ");
                break;

            case OP_NE:
                print_op("NE");
                break;

            case OP_GT:
                print_op("GT");
                break;

            case OP_LT:
                print_op("LT");
                break;

            case OP_GE:
                print_op("GE");
                break;

            case OP_LE:
                print_op("LE");
                break;

            case OP_ODD:
                print_op("ODD");
                break;
        }
        std::cout << std::endl;
        index++;
    }
}
