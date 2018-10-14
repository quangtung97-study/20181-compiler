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

    auto result = scan(input);
    for (auto e: result.tokens) {
        std::cout << token_data_to_string(result, e) << std::endl;
    }
    return 0;
}
