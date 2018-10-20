#include <string>
#include <vector>
#include <utility>
#include "token.hpp"

struct TokenData {
    Token token;
    int input_index;

    // additional data index
    int data_index;
};

struct ScannerResult {
    std::vector<TokenData> tokens;
    std::vector<std::string> ident_names;
    std::vector<int> numbers;
};

struct LineColNumber {
    int line, col;
};

ScannerResult scan(const std::string& input);

std::string to_string(
        const ScannerResult& result, TokenData token);

LineColNumber get_line_col_number(const std::string& input, int input_index);

enum ScannerErrorCode {
    SCANNER_UNRECOGNIZED_CHARACTER,
    SCANNER_NUMBER_TOO_LARGE,
};

struct ScannerError {
    ScannerErrorCode code;
    int input_index;
};


