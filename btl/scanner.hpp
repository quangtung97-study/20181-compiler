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

ScannerResult scan(const std::string& input);

std::string token_data_to_string(
        const ScannerResult& result, TokenData token);
