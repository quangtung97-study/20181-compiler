#include <cstdio>
#include <iostream>
#include "opcode.hpp"
#include <vector>
#include <cassert>

#define MAX_ELEM 256
#define MAX_STACK 10240

int stack[MAX_STACK];

static void run(const std::vector<Instruction>& instructions) {
    int pc = 0;
    int sp = -1;

    while (true) {
        Instruction i = instructions[pc];
        pc++;

        int addr;
        switch (i.op) {
            case OP_LC:
                sp++; stack[sp] = i.p;
                break;

            case OP_INC:
                sp += i.p;
                break;

            case OP_DEC:
                sp -= i.p;
                break;

            case OP_RI:
                addr = stack[sp]; sp--;
                std::cin >> stack[addr];
                break;

            case OP_WI:
                addr = stack[sp]; sp--;
                std::cout << stack[addr] << std::endl;
                break;

            case OP_HALT:
                return;

            default:
                assert(false);
                break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Thieu tham so ten file!" << std::endl;
        return -1;
    }

    FILE *file = std::fopen(argv[1], "rb");
    if (file == nullptr) {
        std::cout << "Khong the doc file!" << std::endl;
        return -1;
    }

    Instruction buff[MAX_ELEM];
    std::vector<Instruction> instructions;
    while (true) {
        int amount = std::fread(buff, 
                sizeof(Instruction), MAX_ELEM, file);
        for (int i = 0; i < amount; i++)
            instructions.push_back(buff[i]);

        if (amount < MAX_ELEM)
            break;
    }

    run(instructions);

    std::fclose(file);
    return 0;
}
