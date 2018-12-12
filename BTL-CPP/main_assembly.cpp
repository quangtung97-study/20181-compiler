#include <iostream>
#include <cstdio>
#include "assembly.hpp"

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Thieu tham so ten file!" << std::endl;
        return -1;
    }

    FILE *file = std::fopen(argv[1], "wb");
    if (file == nullptr) {
        std::cout << "Khong the ghi file!" << std::endl;
        return -1;
    }

    as_init();

    as_set_main(as_code_addr());
    as_inc(1);
    as_load_const(0);
    as_read_int();
    as_load_const(0);
    as_load_indirect();
    as_write_int();
    as_dec(1);
    as_halt();

    as_save(file);

    std::fclose(file);
    return 0;
}
