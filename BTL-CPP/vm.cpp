#include <cstdio>
#include <iostream>
#include "opcode.hpp"
#include <vector>
#include <cassert>

#define MAX_STACK 10240

static int stack[MAX_STACK];
static int pc, sp, bp;

static int RA() {
    return stack[bp + RA_OFFSET];
}

static int BP() {
    return stack[bp + BP_OFFSET];
}

static int base(int p) {
    int result = bp;
    for (int i = 0; i < p; i++) {
        result = stack[result + SL_OFFSET];
    }
    return result;
}

void vm_run(const Instruction *instructions) {
    pc = 0;
    sp = -1;
    bp = 0;

    while (true) {
        Instruction i = instructions[pc];
        pc++;

        int addr;
        switch (i.op) {
            case OP_LA:
                sp++;
                stack[sp] = base(i.p) + i.q;
                break;

            case OP_LV:
                sp++;
                stack[sp] = stack[base(i.p) + i.q];
                break;

            case OP_LC:
                sp++; stack[sp] = i.p;
                break;

            case OP_LI:
                stack[sp] = stack[stack[sp]];
                break;

            case OP_INC:
                sp += i.p;
                break;

            case OP_DEC:
                sp -= i.p;
                break;

            case OP_JMP:
                pc = i.p;
                break;

            case OP_FJMP:
                if (stack[sp] == 0) {
                    pc = i.p;
                }
                sp--;
                break;

            case OP_HALT:
                return;

            case OP_STORE:
                sp -= 2;
                stack[stack[sp + 1]] = stack[sp + 2];
                break;

            case OP_CALL:
                stack[sp + BP_OFFSET + 1] = bp;
                stack[sp + RA_OFFSET + 1] = pc;
                stack[sp + SL_OFFSET + 1] = base(i.p);
                pc = i.q;
                bp = sp + 1;
                break;

            case OP_RET:
                sp = bp - 1;
                pc = RA();
                bp = BP();
                break;

            case OP_RI:
                addr = stack[sp]; sp--;
                std::cin >> stack[addr];
                break;

            case OP_WI:
                std::cout << stack[sp] << " ";
                sp--;
                break;

            case OP_WL:
                std::cout << std::endl;
                break;

            case OP_ADD:
                sp--;
                stack[sp] = stack[sp] + stack[sp + 1];
                break;

            case OP_SUB:
                sp--;
                stack[sp] = stack[sp] - stack[sp + 1];
                break;

            case OP_MUL:
                sp--;
                stack[sp] = stack[sp] * stack[sp + 1];
                break;

            case OP_DIV:
                sp--;
                stack[sp] = stack[sp] / stack[sp + 1];
                break;

            case OP_MOD:
                sp--;
                stack[sp] = stack[sp] % stack[sp + 1];
                break;

            case OP_NEG:
                stack[sp] = -stack[sp];
                break;

            case OP_ODD:
                stack[sp] = (stack[sp] % 2 == 1);
                break;

            case OP_EQ:
                sp--;
                stack[sp] = stack[sp] == stack[sp + 1];
                break;

            case OP_NE:
                sp--;
                stack[sp] = stack[sp] != stack[sp + 1];
                break;

            case OP_GT:
                sp--;
                stack[sp] = stack[sp] > stack[sp + 1];
                break;

            case OP_GE:
                sp--;
                stack[sp] = stack[sp] >= stack[sp + 1];
                break;

            case OP_LT:
                sp--;
                stack[sp] = stack[sp] < stack[sp + 1];
                break;

            case OP_LE:
                sp--;
                stack[sp] = stack[sp] <= stack[sp + 1];
                break;
        }
    }
}
