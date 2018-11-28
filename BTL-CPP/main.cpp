#include "parser.hpp"
#include "scope.hpp"
#include <fstream>

void print_scope(Scope *scope) {
    for (auto& e: scope->names) {
        if (e.kind == KIND_CONST) {
            std::cout << e.name << " = " << e.const_value << std::endl;
        }
        else if (e.kind == KIND_VAR) {
            std::cout << e.name << ", offset: " << e.offset;
            std::cout << ", type: " << "int";
            if (e.var_type.is_array) 
                std::cout << "[" << e.var_type.array_size << "]";
            if (e.var_type.is_reference)
                std::cout << "  ref";
            std::cout << std::endl;
        }
        else if (e.kind == KIND_PROC) {
            std::cout << "=======================================" << std::endl;
            std::cout << "PROCEDURE " << e.name << ": " << 
                e.proc_type.param_types.size() << " paramters" << std::endl;
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
    std::ifstream file(argv[1], std::ios::binary);
    if (file.fail()) {
        std::cout << "Khong the doc file" << std::endl;
        return -1;
    }

    ps_init(file);
    ps_parse();

    std::cout << "Nhan dien thanh cong!!!\n" << std::endl;

    print_scope(scope_top());
    return 0;
}
