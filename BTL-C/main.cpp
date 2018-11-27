#include "parser.h"
#include "scope.h"
#include <iostream>

void print_scope(Scope *scope) {
    for (auto& e: scope->names) {
        if (e.type == TYPE_CONST) {
            std::cout << e.name << " = " << e.const_value << std::endl;
        }
        else if (e.type == TYPE_INT) {
            std::cout << e.name << ", offset: " << e.offset << std::endl;
        }
        else if (e.type == TYPE_PROC) {
            std::cout << "=======================================" << std::endl;
            print_scope(e.proc_scope.get());
            std::cout << "=======================================" << std::endl;
        }
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Thieu tham so\n");
        return -1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("Khong the doc file\n");
        return -1;
    }

    ps_init(fp);
    ps_parse();

    printf("Nhan dien thanh cong!!!\n");

    print_scope(scope_top());

    fclose(fp);
    return 0;
}
