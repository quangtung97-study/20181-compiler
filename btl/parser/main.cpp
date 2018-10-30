#include "parser.hpp"
#include <iostream>
#include <fstream>

std::string input_string(int argc, char **argv) {
    std::string s;
    if (argc <= 1) {
        std::cout << "Thieu ten file" << std::endl;
        exit(1);
    }
    else {
        auto filename = argv[1];
        std::ifstream file(filename);
        if (!file) {
            std::cout << "Khong doc duoc file!!!" << std::endl;
            exit(1);
        }
        while (!file.eof()) {
            char buff[1025];
            file.read(buff, 2014);
            buff[file.gcount()] = '\0';
            s += buff;
        }
    }
    return s;
}

int main(int argc, char **argv) {
    auto s = input_string(argc, argv);
    try {
        parse(scan(s));
        std::cout << "Da nhan dien duoc chuong trinh!!!" << std::endl;
    } 
    catch (const ScannerError& e) {
        if (e.code == SCANNER_NUMBER_TOO_LARGE) {
            std::cout << "LOI SCANNER: " 
                << "So qua lon!!!" << std::endl;
        }
        else if (e.code == SCANNER_UNRECOGNIZED_CHARACTER) {
            std::cout << "LOI SCANNER: " 
                << "Khong nhan dien duoc ki tu!!!" << std::endl;
        }
        auto linecol = get_line_col_number(s, e.input_index);
        std::cout << "Dong: " << linecol.line << std::endl;
        std::cout << "Cot: " << linecol.col << std::endl;
        return 1;
    }
    catch (const ParserError& e) {
        std::cout << "LOI PARSER: " 
            << e.what << "!!!" << std::endl;

        auto linecol = get_line_col_number(s, e.input_index);
        std::cout << "Dong: " << linecol.line << std::endl;
        std::cout << "Cot: " << linecol.col << std::endl;
    }
    return 0;
}
