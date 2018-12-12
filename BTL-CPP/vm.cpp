#include <cstdio>
#include <iostream>
#include "opcode.hpp"
#include <vector>
#include <cassert>

#define MAX_ELEM 256
#define MAX_STACK 10240

int stack[MAX_STACK];
int pc, sp, bp;

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

static void RA_CALLING(int addr) {
    stack[sp + CALL_SIZE + 1 + RA_OFFSET] = addr;
}

static void BP_CALLING(int addr) {
    stack[sp + CALL_SIZE + 1 + BP_OFFSET] = addr;
}

static void SL_CALLING(int addr) {
    stack[sp + CALL_SIZE + 1 + SL_OFFSET] = addr;
}

static void run(const std::vector<Instruction>& instructions) {
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
                // std::cout << "DEBUG: LA: " << i.p << " " << i.q << " addr: " << stack[sp] << std::endl;
                break;

            case OP_LV:
                sp++;
                stack[sp] = stack[base(i.p) + i.q];
                // std::cout << "DEBUG: LV: " << i.p << " " << i.q << " addr: " << stack[sp] << std::endl;
                break;

            case OP_LC:
                sp++; stack[sp] = i.p;
                // std::cout << "DEBUG: LC: " << i.p << std::endl;
                break;

            case OP_LI:
                stack[sp] = stack[stack[sp]];
                // std::cout << "DEBUG: LI: " << stack[sp] << std::endl;
                break;

            case OP_INC:
                sp += i.p;
                // std::cout << "DEBUG: INC: " << i.p << std::endl;
                break;

            case OP_DEC:
                sp -= i.p;
                // std::cout << "DEBUG: DEC: " << i.p << std::endl;
                break;

            case OP_JMP:
                pc = i.p;
                // std::cout << "DEBUG: JMP: " << i.p << std::endl;
                break;

            case OP_FJMP:
                if (stack[sp] == 0) {
                    pc = i.p;
                    // std::cout << "DEBUG: FJMP: " << i.p << std::endl;
                }
                sp--;
                break;

            case OP_HALT:
                return;

            case OP_STORE:
                sp -= 2;
                stack[stack[sp + 1]] = stack[sp + 2];
                // std::cout << "DEBUG: STORE: " << stack[sp + 1] << " " << stack[sp + 2] << std::endl;
                break;

            case OP_CALL:
                SL_CALLING(base(i.p));
                BP_CALLING(bp);
                RA_CALLING(pc);
                pc = i.q;
                sp += CALL_SIZE;
                bp = sp + 1;
                // std::cout << "DEBUG: CALL: " << i.q << std::endl;
                break;

            case OP_RET:
                sp -= CALL_SIZE;
                pc = RA();
                bp = BP();
                // std::cout << "DEBUG: RET: " << std::endl;
                break;

            case OP_RI:
                addr = stack[sp]; sp--;
                std::cin >> stack[addr];
                break;

            case OP_WI:
                std::cout << stack[sp] << std::endl;
                sp--;
                break;

            case OP_WL:
                std::cout << std::endl;
                break;

            case OP_ADD:
                sp--;
                stack[sp] = stack[sp] + stack[sp + 1];
                // std::cout << "DEBUG: ADD: " << stack[sp] << std::endl;
                break;

            case OP_SUB:
                sp--;
                stack[sp] = stack[sp] - stack[sp + 1];
                // std::cout << "DEBUG: SUB: " << stack[sp] << std::endl;
                break;

            case OP_MUL:
                sp--;
                stack[sp] = stack[sp] * stack[sp + 1];
                // std::cout << "DEBUG: MUL: " << stack[sp] << std::endl;
                break;

            case OP_DIV:
                sp--;
                stack[sp] = stack[sp] / stack[sp + 1];
                // std::cout << "DEBUG: DIV: " << stack[sp] << std::endl;
                break;

            case OP_MOD:
                sp--;
                stack[sp] = stack[sp] % stack[sp + 1];
                // std::cout << "DEBUG: MOD: " << stack[sp] << std::endl;
                break;

            case OP_NEG:
                stack[sp] = -stack[sp];
                break;

            case OP_ODD:
                stack[sp] = (stack[sp] % 2 == 1);
                // std::cout << "DEBUG: COND ODD: " << stack[sp] << std::endl;
                break;

            case OP_EQ:
                sp--;
                stack[sp] = stack[sp] == stack[sp + 1];
                // std::cout << "DEBUG: COND EQ: " << stack[sp] << std::endl;
                break;

            case OP_NE:
                sp--;
                stack[sp] = stack[sp] != stack[sp + 1];
                // std::cout << "DEBUG: COND NE: " << stack[sp] << std::endl;
                break;

            case OP_GT:
                sp--;
                stack[sp] = stack[sp] > stack[sp + 1];
                // std::cout << "DEBUG: COND GT: " << stack[sp] << std::endl;
                break;

            case OP_GE:
                sp--;
                stack[sp] = stack[sp] >= stack[sp + 1];
                // std::cout << "DEBUG: COND GE: " << stack[sp] << std::endl;
                break;

            case OP_LT:
                sp--;
                stack[sp] = stack[sp] < stack[sp + 1];
                // std::cout << "DEBUG: COND LT: " << stack[sp] << std::endl;
                break;

            case OP_LE:
                sp--;
                stack[sp] = stack[sp] <= stack[sp + 1];
                // std::cout << "DEBUG: COND LE: " << stack[sp] << std::endl;
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

    std::fread(&pc, sizeof(pc), 1, file);

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
