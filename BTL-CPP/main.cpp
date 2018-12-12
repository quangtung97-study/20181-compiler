#include "parser.hpp"
#include "scope.hpp"
#include "assembly.hpp"
#include <iostream>

void print_scope(Scope *scope) {
    if (scope->parent != nullptr) {
        std::cout << "Parameters: " << std::endl;
    }
    for (auto& e: scope->params) {
        std::cout << e.name << ", offset: " << e.offset;
        std::cout << ", type: " << "int";
        if (e.var_type == VAR_REF)
            std::cout << "  ref";
        std::cout << std::endl;
    }

    if (scope->parent != nullptr) {
        std::cout << "Declarations: " << std::endl;
    }
    for (auto& e: scope->names) {
        if (e.kind == KIND_CONST) {
            std::cout << e.name << " = " << e.const_value << std::endl;
        }
        else if (e.kind == KIND_VAR) {
            std::cout << e.name << ", offset: " << e.offset;
            std::cout << ", type: " << "int";
            if (e.var_type == VAR_ARRAY)
                std::cout << "[" << e.array_size << "]";
            std::cout << std::endl;
        }
        else if (e.kind == KIND_PROC) {
            std::cout << "=======================================" << std::endl;
            std::cout << "PROCEDURE " << e.name << ": " << 
                e.proc_scope->params.size() << " paramters" << std::endl;
            std::cout << "---------------------------------------" << std::endl;
            print_scope(e.proc_scope.get());
            std::cout << "=======================================" << std::endl;
        }
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Thieu tham so" << std::endl;
        return -1;
    }

    FILE *file = std::fopen(argv[1], "rb");
    if (file == NULL) {
        std::cout << "Khong the doc file" << std::endl;
        return -1;
    }

    as_init();
    ps_init(file);
    ps_parse();

    std::cout << "Nhan dien thanh cong!!!" << std::endl;

    FILE *output;
    if (argc >= 3) {
        output = std::fopen(argv[2], "wb");
    }
    else {
        output = std::fopen("a.out", "wb");
    }
    as_save(output);
    std::fclose(output);

    // print_scope(scope_top());
    
    std::fclose(file);
    return 0;
}
