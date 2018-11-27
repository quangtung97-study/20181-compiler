#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

enum Token {
    TOKEN_IDENT = 1,
    TOKEN_NUMBER,

    TOKEN_BEGIN, TOKEN_CALL, TOKEN_CONST, TOKEN_DO, 
    TOKEN_ELSE, TOKEN_END, 
    TOKEN_FOR, TOKEN_IF, TOKEN_ODD, 
    TOKEN_PROCEDURE, TOKEN_PROGRAM, 
    TOKEN_THEN, TOKEN_TO, 
    TOKEN_VAR, TOKEN_WHILE, 

    TOKEN_PLUS, TOKEN_MINUS, TOKEN_TIMES, TOKEN_DIVIDE, TOKEN_REMAINDER,

    TOKEN_EQ, TOKEN_NE, TOKEN_LT, TOKEN_GT, TOKEN_LE, TOKEN_GE,

    TOKEN_LPARENT, TOKEN_RPARENT, 
    TOKEN_LBRACKET, TOKEN_RBRACKET,

    TOKEN_PERIOD, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_ASSIGN,
    END_OF_TOKENS
};

void sc_init(FILE *fp);

enum Token sc_get();
const char *sc_name();
int sc_number();
int sc_line();
int sc_col();

void sc_next();

void sc_print(enum Token t);

#endif
