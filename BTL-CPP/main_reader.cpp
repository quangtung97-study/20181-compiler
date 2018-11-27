#include "reader.h"
#include <ctype.h>
#include <stdio.h>
#include <iostream>

void show(char ch) {
    if (ch == '\n')
        std::cout << "LF";
    else if (ch == ' ')
        std::cout << "SPC";
    else if (ch == '\r')
        std::cout << "CR";
    else if (ch == '\0')
        std::cout << "NULL";
    else {
        std::cout << ch;
    }
}

int main() {
    FILE *fp = fopen("input", "rb");
    if (fp == NULL) {
        std::cout << "Loi trong qua trinh doc file" << std::endl;
        return -1;
    }

    rd_set(fp);
    while (rd_get() != '\0') {
        show(rd_get());
        std::cout << "Pos: " << rd_line() 
            << ", " << rd_col() << std::endl;
        rd_next();
    }

    auto s = rd_all();
    std::cout << s << std::endl;

    fclose(fp);
    return 0;
}
