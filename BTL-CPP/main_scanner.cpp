#include "scanner.hpp"
#include <fstream>

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

    sc_init(file);

    while (sc_get() != END_OF_TOKENS) {
        sc_print(sc_get());
        sc_next();
    }
    return 0;
}
