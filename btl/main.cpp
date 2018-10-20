#include <iostream>
#include "scanner.hpp"
#include <fstream>


int main(int argc, char **argv) {
    if (argc == 1) 
        return -1;

    auto path = argv[1];
    std::ifstream file{path};

    if (file.fail())
        return -1;

    std::string input;
    char buf[1025];

    while (!file.eof()) {
        file.read(buf, 1024);
        auto count = file.gcount();
        buf[count] = '\0';
        input += buf;
    }

    try {
        auto result = scan(input);
        for (auto e: result.tokens) {
            std::cout << to_string(result, e) << std::endl;
        }
    } 
    catch (const ScannerError& e) {
        if (e.code == SCANNER_UNRECOGNIZED_CHARACTER)
            std::cout << "Khong nhan dien duoc ki tu!!!" << std::endl;
        else if (e.code == SCANNER_NUMBER_TOO_LARGE) 
            std::cout << "So qua lon (Khong duoc vuot qua 999'999'999)!!!" << std::endl;
        auto linecol = get_line_col_number(input, e.input_index);
        std::cout << "Tai dong: " << linecol.line << 
            ", cot: " << linecol.col << std::endl;
        return -1;
    }
    return 0;
}
