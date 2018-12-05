#include "reader.hpp"
#include <iostream>

void show(char ch) {
    if (ch == '\n')
        std::cout << "LF" << '\t';
    else if (ch == ' ')
        std::cout << "SPC" << '\t';
    else if (ch == '\r')
        std::cout << "CR" << '\t';
    else if (ch == '\0')
        std::cout << "NULL" << '\t';
    else {
        std::cout << ch << '\t';
    }
}

int main() {
    FILE *file = std::fopen("../input", "rb");

    if (!file) {
        std::cout << "Loi trong qua trinh doc file" << std::endl;
        return -1;
    }

    rd_set(file);
    while (rd_get() != '\0') {
        show(rd_get());
        std::cout << "Pos: " << rd_line() 
            << ", " << rd_col() << std::endl;
        rd_next();
    }

    rd_reset();
    std::string line;
    while (rd_line(line)) {
        std::cout << line << std::endl;
    }
    return 0;
}
