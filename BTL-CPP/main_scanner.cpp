#include "scanner.hpp"
#include <iostream>

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Thieu tham so" << std::endl;
        return -1;
    }
    FILE *file = std::fopen(argv[1], "rb");
    if (!file) {
        std::cout << "Khong the doc file" << std::endl;
        return -1;
    }

    sc_init(file);

    while (sc_get() != END_OF_TOKENS) {
        sc_print(sc_get());
        sc_next();
    }
    return 0;
}
