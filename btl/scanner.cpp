#include "scanner.hpp"
#include <array>
#include <iostream>
#include <map>

enum State {
    START = 0,
    NAME, NUMBER,

    PLUS, MINUS, TIMES, DIVIDE, REMAINDER,
    PERIOD, COMMA, SEMICOLON, 
    LPARENT, RPARENT, 
    LBRACKET, RBRACKET,

    EQ, LT, GT, 
    NE, LE, GE,

    COLON,
    ASSIGN,

    STATE_SIZE,
};

static std::map<std::string, Token> init_token_map() {
    std::map<std::string, Token> map;
    
    map["begin"] = Token::Begin;
    map["call"] = Token::Call;
    map["const"] = Token::Const;
    map["do"] = Token::Do;
    map["else"] = Token::Else;
    map["end"] = Token::End;
    map["for"] = Token::For;
    map["if"] = Token::If;
    map["odd"] = Token::Odd;
    map["procedure"] = Token::Procedure;
    map["function"] = Token::Function;
    map["program"] = Token::Program;
    map["then"] = Token::Then;
    map["to"] = Token::To;
    map["var"] = Token::Var;
    map["while"] = Token::While;

    return map;
}

static const std::map<std::string, Token> token_map = init_token_map();

// ------------------------------------------------------------------
typedef bool HandleResult;
const HandleResult Continue = true;
const HandleResult Terminated = false;

typedef HandleResult (*HandleFunction)(State&, char, int);
typedef std::array<HandleFunction, STATE_SIZE> Delta;

// ------------------------------------------------------------------
// Global Variables
static std::string g_data;
static Token g_token;
static std::string g_ident_name;
static int g_number;
static int g_token_index;

static HandleResult handle_start(State& state, char ch, int index) {
    if (std::isspace(ch) || ch == '\0') 
        return Continue;

    auto continue_with = [&] (State new_state) {
        state = new_state;
        g_data += ch;
        g_token_index = index;
        return Continue;
    };

    if (std::isalpha(ch) || ch == '_') return continue_with(NAME);
    if (std::isdigit(ch))return continue_with(NUMBER);

    if (ch == '+') return continue_with(PLUS);
    if (ch == '-') return continue_with(MINUS);
    if (ch == '*') return continue_with(TIMES);
    if (ch == '/') return continue_with(DIVIDE);
    if (ch == '%') return continue_with(REMAINDER);

    if (ch == '.') return continue_with(PERIOD);
    if (ch == ',') return continue_with(COMMA);
    if (ch == ';') return continue_with(SEMICOLON);

    if (ch == '(') return continue_with(LPARENT);
    if (ch == ')') return continue_with(RPARENT);
    if (ch == '[') return continue_with(LBRACKET);
    if (ch == ']') return continue_with(RBRACKET);

    if (ch == '=') return continue_with(EQ);
    if (ch == '>') return continue_with(GT);
    if (ch == '<') return continue_with(LT);

    if (ch == ':') return continue_with(COLON);

    throw std::runtime_error("Unrecognize Character");
}

static HandleResult handle_name(State&, char ch, int) {
    if (std::isalpha(ch) || std::isdigit(ch) || ch == '_') {
        g_data += ch;
        return Continue;
    }

    auto it = token_map.find(g_data);
    if (it != token_map.end()) {
        g_token = it->second;
    }
    else {
        g_token = Token::Ident;
        g_ident_name = g_data.substr(0, 10);
    }
    return Terminated;
}

static HandleResult handle_number(State&, char ch, int) {
    if (std::isdigit(ch)) {
        g_data += ch;
        return Continue;
    }

    g_token = Token::Number;
    if (g_data.size() > MAX_DIGIT_COUNT)
        throw std::runtime_error("Number is too large");
    g_number = std::stoi(g_data);
    return Terminated;
}

template <Token token_value>
static HandleResult handle(State&, char, int) {
    g_token = token_value;
    return Terminated;
}

static HandleResult handle_LT(State& state, char ch, int) {
    if (ch == '=') {
        g_data += ch;
        state = LE;
        return Continue;
    }
    
    if (ch == '>') {
        g_data += ch;
        state = NE;
        return Continue;
    }

    g_token = Token::LT;
    return Terminated;
}

static HandleResult handle_GT(State& state, char ch, int) {
    if (ch == '=') {
        g_data += ch;
        state = GE;
        return Continue;
    }

    g_token = Token::GT;
    return Terminated;
}

static HandleResult handle_colon(State& state, char ch, int) {
    if (ch == '=') {
        g_data += ch;
        state = ASSIGN;
        return Continue;
    }
    throw std::runtime_error("Unrecognize character");
}

static Delta init_delta() {
    std::array<HandleFunction, STATE_SIZE> delta;

    delta[START] = handle_start;
    delta[NAME] = handle_name;
    delta[NUMBER] = handle_number;

    delta[PLUS] = handle<Token::Plus>;
    delta[MINUS] = handle<Token::Minus>;
    delta[TIMES] = handle<Token::Times>;
    delta[DIVIDE] = handle<Token::Divide>;
    delta[REMAINDER] = handle<Token::Remainder>;

    delta[PERIOD] = handle<Token::Period>;
    delta[COMMA] = handle<Token::Comma>;
    delta[SEMICOLON] = handle<Token::Semicolon>;

    delta[LPARENT] = handle<Token::LParent>;
    delta[RPARENT] = handle<Token::RParent>;
    delta[LBRACKET] = handle<Token::LBracket>;
    delta[RBRACKET] = handle<Token::RBracket>;

    delta[EQ] = handle<Token::EQ>;

    delta[LT] = handle_LT;
    delta[GT] = handle_GT;
    delta[COLON] = handle_colon;
    delta[NE] = handle<Token::NE>;
    delta[LE] = handle<Token::LE>;
    delta[GE] = handle<Token::GE>;
    delta[ASSIGN] = handle<Token::Assign>;

    return delta;
}

static const Delta delta = init_delta();

static HandleResult step(State& state, char ch, int index) {
    return delta[state](state, ch , index);
}

ScannerResult scan(const std::string& input) {
    std::vector<TokenData> tokens;
    std::vector<std::string> ident_names;
    std::vector<int> numbers;

    State state = START;

    auto handle = [&](char ch, size_t& index) {
        auto result = step(state, ch, index);
        if (result == Terminated) {

            int data_index = -1;

            if (g_token == Token::Ident) {
                data_index = ident_names.size();
                ident_names.push_back(g_ident_name);
            }
            else if (g_token == Token::Number) {
                data_index = numbers.size();
                numbers.push_back(g_number);
            }

            tokens.push_back({g_token, (int)g_token_index, data_index});

            state = START;
            g_data.clear();
            index--;
        }
    };

    size_t i = 0;
    for (; i < input.length(); i++) {
        handle(input[i], i);
    }

    handle('\0', i);

    return {tokens, ident_names, numbers};
}

std::string to_string(const ScannerResult& result, TokenData data) {
    auto s = token_to_string(data.token);
    std::string tmp = s;
    tmp += ":\t";

    if (data.token == Token::Ident)
        return tmp + result.ident_names[data.data_index];
    if (data.token == Token::Number)
        return tmp + std::to_string(result.numbers[data.data_index]);

    return s;
}
