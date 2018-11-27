#include "scanner.h"
#include <ctype.h>
#include <iostream>

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Thieu tham so" << std::endl;
        return -1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        std::cout << "Khong the doc file" << std::endl;
        return -1;
    }

    sc_init(fp);

    while (sc_get() != END_OF_TOKENS) {
        sc_print(sc_get());
        sc_next();
    }

    fclose(fp);
    return 0;
}
