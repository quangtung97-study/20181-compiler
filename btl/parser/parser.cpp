#include "parser.hpp"
#include <iostream>

ScannerResult g_scanner_result;
size_t g_index;

void init(ScannerResult res) {
    g_scanner_result = std::move(res);
    g_index = 0;
}

void next() {
    g_index++;
}

const TokenData get() {
    return g_scanner_result.tokens[g_index];
}

bool has_no_remain() {
    return g_index >= g_scanner_result.tokens.size() - 1;
}

void ERROR(const char *s) {
    throw ParserError(s, get().input_index);
}

void NEXT(Token token, const char *error_msg) {
    if (get().token != token)
        ERROR(error_msg);
    next();
}

void EXPR();

void FACTOR() {
    if (get().token == Token::Ident) {
        next();
        if (get().token == Token::LBracket) {
            next();
            EXPR();
            NEXT(Token::RBracket, "Thieu \"]\"");
        }
    }
    else if (get().token == Token::Number) {
        next();
    }
    else if (get().token == Token::LParent) {
        next();
        EXPR();
        NEXT(Token::RParent, "Thieu \")\"");
    }
    else {
        ERROR("Thieu toan hang");
    }
}

void TERM() {
    FACTOR();
term_loop:
    if (get().token == Token::Times) {
        next();
        FACTOR();
        goto term_loop;
    }
    if (get().token == Token::Remainder) {
        next();
        FACTOR();
        goto term_loop;
    }
    if (get().token == Token::Divide) {
        next();
        FACTOR();
        goto term_loop;
    }
}

void EXPR() {
    if (get().token == Token::Plus) {
        next();
    }
    if (get().token == Token::Minus) {
        next();
    }
    TERM();
    while (get().token == Token::Plus || 
            get().token == Token::Minus) {
        next();
        TERM();
    }
}

void CONDITION() {
    if (get().token == Token::Odd) {
        next();
        EXPR();
    }
    else {
        EXPR();
        if (get().token == Token::EQ)
            next();
        else if (get().token == Token::GT)
            next();
        else if (get().token == Token::GE)
            next();
        else if (get().token == Token::LT)
            next();
        else if (get().token == Token::LE)
            next();
        else if (get().token == Token::NE) 
            next();
        else 
            ERROR("Thieu mot phep toan so sanh");
        EXPR();
    }
}

void STATEMENT();
void BEGIN();

void ASSIGN_STMT() {
    if (get().token == Token::LBracket) {
        next();
        EXPR();
        NEXT(Token::RBracket, "Thieu \"]\"");
    }

    NEXT(Token::Assign, "Thieu dau gan := ");
    EXPR();
}

void CALL_STMT() {
    NEXT(Token::Ident, "Thieu ten thu tuc duoc goi");
    if (get().token == Token::LParent) {
        next();
next_arg:
        EXPR();
        if (get().token == Token::Comma) {
            next();
            goto next_arg;
        }
        NEXT(Token::RParent, "Thieu dau \")\"");
    }
}

void IF_STMT() {
    CONDITION();
    NEXT(Token::Then, "Thieu THEN");
    STATEMENT();
    if (get().token == Token::Else) {
        next();
        STATEMENT();
    }
}

void WHILE_STMT() {
    CONDITION();
    NEXT(Token::Do, "Thieu DO");
    STATEMENT();
}

void FOR_STMT() {
    NEXT(Token::Ident, "Thieu ten bien chay FOR");
    NEXT(Token::Assign, "Thieu phep gan := ");
    EXPR();
    NEXT(Token::To, "Thieu TO");
    EXPR();
    NEXT(Token::Do, "Thieu DO");
    STATEMENT();
}

void STATEMENT() {
    if (get().token == Token::Ident) {
        next();
        ASSIGN_STMT();
    }
    else if (get().token == Token::Call) {
        next();
        CALL_STMT();
    }
    else if (get().token == Token::If) {
        next();
        IF_STMT();
    }
    else if (get().token == Token::While) {
        next();
        WHILE_STMT();
    }
    else if (get().token == Token::For) {
        next();
        FOR_STMT();
    }
    else if (get().token == Token::Begin) {
        next();
        BEGIN();
    }
}

void BLOCK();

void PROCEDURE() {
    NEXT(Token::Ident, "Thieu ten thu tuc");
    if (get().token == Token::LParent) {
        next();
loop_args:
        if (get().token == Token::Var)
            next();
        NEXT(Token::Ident, "Thieu ten tham so cua thu tuc");
        if (get().token == Token::Semicolon) {
            next();
            goto loop_args;
        }

        NEXT(Token::RParent, "Thieu dau \")\"");
    }

    NEXT(Token::Semicolon, "Thieu dau ; sau khai bao cac tham so thu tuc");
    BLOCK();
    NEXT(Token::Semicolon, "Thieu dau ; ket thuc thu tuc");
}

void BEGIN() {
    STATEMENT();
    while (get().token == Token::Semicolon) {
        next();
        STATEMENT();
    }
    NEXT(Token::End, "Thieu ; hoac END");
}

void VAR() {
    NEXT(Token::Ident, "Thieu ten bien");

    if (get().token == Token::LBracket) {
        next();
        NEXT(Token::Number, "Thieu chi so mang");
        NEXT(Token::RBracket, "Thieu dau \"]\"");
    }

    if (get().token == Token::Comma) {
        next();
        VAR();
    }
    else if (get().token == Token::Semicolon) {
        next();
    }
    else {
        ERROR("Thieu dau cham phay ket thuc khai bao bien");
    }
}

void CONST() {
    NEXT(Token::Ident, "Thieu ten cho hang");
    NEXT(Token::EQ, "Thieu dau =");
    NEXT(Token::Number, "Thieu so nguyen");
    if (get().token == Token::Comma) {
        next();
        CONST();
    }
    else if (get().token == Token::Semicolon) {
        next();
    }
    else {
        ERROR("Thieu dau cham phay ket thuc khai bao hang");
    }
}

void BLOCK() {
    if (get().token == Token::Const) {
        next();
        CONST();
    }

    if (get().token == Token::Var) {
        next();
        VAR();
    }

    while (get().token == Token::Procedure) {
        next();
        PROCEDURE();
    }
    
    if (get().token == Token::Begin) {
        next();
        BEGIN();
    }
    else {
        ERROR("Thieu BEGIN");
    }
}

void PROGRAM() {
    NEXT(Token::Program, "Thieu tu khoa PROGRAM");
    NEXT(Token::Ident, "Thieu ten chuong trinh");
    NEXT(Token::Semicolon, "Thieu dau cham phay");
    BLOCK();
    NEXT(Token::Period, "Thieu dau cham");
}

void parse(ScannerResult res) {
    init(std::move(res));
    PROGRAM();
}

