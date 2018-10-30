#ifndef PARSER_HPP
#define PARSER_HPP

#include "scanner.hpp"
#include <stdexcept>

struct ParserError {
    ParserError(const std::string& error, int index)
        : what(error), input_index(index) {}
    std::string what;
    int input_index;
};

void parse(ScannerResult result);

#endif
