#include "scanner.hpp"
#include <array>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

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
static int g_input_index;

static HandleResult handle_start(State& state, char ch, int index) {
    if (std::isspace(ch) || ch == '\0') 
        return Continue;

    auto continue_with = [&] (State new_state) {
        state = new_state;
        g_data += ch;
        g_input_index = index;
        return Continue;
    };

    // if (std::isalpha(ch) || ch == '_') return continue_with(NAME);
    if (std::isalpha(ch)) return continue_with(NAME);
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

    throw ScannerError{SCANNER_UNRECOGNIZED_CHARACTER, index};
}

void str_tolower(std::string& s) {
    for (auto& e: s)
        e = std::tolower(e);
}

static HandleResult handle_name(State&, char ch, int) {
    // if (std::isalpha(ch) || std::isdigit(ch) || ch == '_') {
    if (std::isalpha(ch) || std::isdigit(ch)) {
        g_data += ch;
        return Continue;
    }

    str_tolower(g_data);

    auto it = token_map.find(g_data);
    if (it != token_map.end()) {
        g_token = it->second;
    }
    else {
        g_token = Token::Ident;
        g_ident_name = g_data.substr(0, IDENT_NAME_SIZE);
    }
    return Terminated;
}

static HandleResult handle_number(State&, char ch, int) {
    if (std::isdigit(ch)) {
        g_data += ch;
        return Continue;
    }

    g_token = Token::Number;

    auto it = std::find_if(g_data.begin(), g_data.end(), 
            [] (char ch) { return ch != '0'; });
    if (g_data.end() - it > MAX_DIGIT_COUNT)
        throw ScannerError{SCANNER_NUMBER_TOO_LARGE, g_input_index};

    std::stringstream ss;
    ss << g_data;
    ss >> g_number;

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

static HandleResult handle_colon(State& state, char ch, int index) {
    if (ch == '=') {
        g_data += ch;
        state = ASSIGN;
        return Continue;
    }
    throw ScannerError{SCANNER_UNRECOGNIZED_CHARACTER, index};
}

// Transition function
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

static void remove_between(std::string& str, 
        const std::string& left, const std::string& right) {
    auto it = str.begin();
    while (true) {
        it = std::search(it, str.end(), left.begin(), left.end());
        if (it == str.end())
            break;

        auto first = it;
        it = std::search(first + 1, str.end(), right.begin(), right.end());
        if (it != str.end())
            it += right.length();

        str.erase(first, it);
    }
}

static void next_it_range_between(
        std::string::const_iterator it,
        std::string::const_iterator end,
        const std::string& left, 
        const std::string& right,
        std::string::const_iterator& first, 
        std::string::const_iterator& last) 
{
    it = std::search(it, end, left.begin(), left.end());
    first = it;
    if (it != end)
        it += left.length();

    it = std::search(it, end, right.begin(), right.end());
    if (it != end)
        it += right.length();

    last = it;
}

static void next_comment_it_range(
        std::string::const_iterator it,
        std::string::const_iterator end,
        std::string::const_iterator& first,
        std::string::const_iterator& last)
{
    const std::string two_slash = "//";
    const std::string new_line = "\n";
    const std::string left_comment = "/*";
    const std::string right_comment = "*/";

    std::string::const_iterator first1, last1, first2, last2;

    next_it_range_between(it, end, two_slash, new_line, first1, last1);
    next_it_range_between(it, end, left_comment, right_comment, first2, last2);

    if (first1 < first2) {
        first = first1;
        last = last1;
    }
    else {
        first = first2;
        last = last2;
    }
}

ScannerResult scan(const std::string& input) {
    std::vector<TokenData> tokens;
    std::vector<std::string> ident_names;
    std::vector<int> numbers;

    State state = START;

    auto handle = [&](char ch, std::string::const_iterator& it) {
        auto result = step(state, ch, it - input.begin());
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

            tokens.push_back({g_token, (int)g_input_index, data_index});

            state = START;
            g_data.clear();
            --it;
        }
    };

    auto it = input.begin();

    std::string::const_iterator comment_first, comment_last;
    next_comment_it_range(
            it, input.end(), comment_first, comment_last);

    for (;it != input.end(); ++it) {
        if (it == comment_first) {
            handle(' ', it);
            it = comment_last;
            next_comment_it_range(
                    it, input.end(), comment_first, comment_last);
            --it;
            continue;
        }
        handle(*it, it);
    }
    handle('\0', it);

    return {tokens, ident_names, numbers};
}

std::string to_string(const ScannerResult& scanner_result, TokenData data) {
    auto s = token_to_string(data.token);
    std::string result = s;
    result += ":\t";

    if (data.token == Token::Ident)
        return result + scanner_result.ident_names[data.data_index];

    if (data.token == Token::Number) {
        std::stringstream ss;
        auto num = scanner_result.numbers[data.data_index];
        ss << num;
        return result + ss.str();
    }

    return s;
}

LineColNumber get_line_col_number(const std::string& input, int input_index) {
    auto it = input.begin();
    int line_index = 0;
    while (it != input.end()) {
        auto newline_it = std::find(it, input.end(), '\n');
        auto range_size = newline_it - it;

        if (newline_it != input.end()) {
            newline_it++;
            range_size++;
        }

        if (input_index < range_size ) 
            return {line_index + 1, input_index + 1};

        line_index++;
        input_index -= range_size;
        it = newline_it;
    }
    return {0, 0};
}
