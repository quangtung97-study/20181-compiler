#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdexcept>

size_t g_index = 0;
std::string g_string;

void init(const std::string& s) {
    g_string = s;
    g_index = 0;
}

char get() {
    return g_string[g_index];
}

bool has_no_remain() {
    return g_index >= g_string.size() - 1;
}

void next() {
    g_index++;
}

std::string input_string(int argc, char **argv) {
    std::string s;
    if (argc <= 1) {
        std::getline(std::cin, s);
    }
    else {
        auto filename = argv[1];
        std::ifstream file(filename);
        while (!file.eof()) {
            char buff[1025];
            file.read(buff, 2014);
            buff[file.gcount()] = '\0';
            s += buff;
        }
    }
    auto it = std::remove_if(s.begin(), s.end(), 
            [] (char ch) { return std::isspace(ch); });
    s.erase(it, s.end());
    s.push_back('\0');
    return s;
}

void E();

void G() {
    if (get() == 'a') {
        next();
    }
    else if (get() == '(') {
        next();
        E();
        if (get() != ')') 
            throw std::runtime_error("Thieu dau dong ngoac");
        next();
    }
    else 
        throw std::runtime_error("Thieu toan hang");
}

void F1() {
    if (get() == '*') {
        next();
        G(); F1();
    }
}

void F() {
    G(); F1();
}

void E1() {
    if (get() == '+') {
        next();
        F(); E1();
    }
}

void E() {
    F(); E1();
}

int main(int argc, char **argv) {
    auto s = input_string(argc, argv);
    init(s);
    try {
        E();
        if (has_no_remain()) {
            std::cout << "-----------------------------" << std::endl;
            std::cout << "Xau duoc chap nhan!!!" << std::endl;
        }
        else {
            std::cout << "LOI: Con thua xau dau vao!!!" << std::endl;
        }
    } 
    catch (const std::runtime_error& e) {
        std::cout << "LOI: " << e.what() << "!!!" << std::endl;
    }
    return 0;
}
