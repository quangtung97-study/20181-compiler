#include "reader.hpp"
#include <fstream>

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
    std::ifstream file("../input", std::ios::binary);

    if (file.fail()) {
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

    auto s = rd_all();
    std::cout << s << std::endl;
    return 0;
}
