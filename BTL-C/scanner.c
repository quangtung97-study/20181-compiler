#include "scanner.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define IDENT_NAME_SIZE 10
#define MAX_DIGIT_COUNT 9

static enum Token g_token;

static char g_name[32];
static int g_name_end;

static char g_number[32];
static int g_number_end;
static int g_number_value;

static void error(const char *s, int line, int col) {
    printf("Loi: %s!!!\n", s);
    printf("Tai dong %d, cot %d\n", line, col);
    exit(-1);
}

void sc_init() {
    g_name_end = 0;
    g_number_end = 0;
    sc_next();
}

enum Token sc_get() {
    return g_token;   
}

static void put_name(char ch) {
    if (g_name_end < IDENT_NAME_SIZE)
        g_name[g_name_end++] = ch;
}

static void name_end() {
    g_name[g_name_end++] = '\0';
    for (int i = 0; i < g_name_end - 1; i++)
        g_name[i] = tolower(g_name[i]);
    g_name_end = 0;
}

const char *sc_name() {
    return g_name;
}

static void put_number(char ch, int line, int col) {
    if (g_number_end == MAX_DIGIT_COUNT)
        error("So qua lon", line, col);

    if (ch != '0' || g_number_end > 0)
        g_number[g_number_end++] = ch;
}

static void number_end() {
    g_number[g_number_end++] = '\0';
    if (g_number[0] == '\0')
        g_number_value = 0;
    else 
        g_number_value = atoi(g_number);
    g_number_end = 0;
}

int sc_number() {
    return g_number_value;
}

static void put_token(enum Token t) {
    g_token = t;
}

#define IF_KEYWORD(str, token) \
    if (strcmp(sc_name(), str) == 0) \
        put_token(token)

static void NAME() {
    while (isalpha(rd_get()) || isdigit(rd_get())) {
        put_name(rd_get());
        rd_next();
    }
    name_end();

    IF_KEYWORD("begin", TOKEN_BEGIN);
    else IF_KEYWORD("call", TOKEN_CALL);
    else IF_KEYWORD("const", TOKEN_CONST);
    else IF_KEYWORD("do", TOKEN_DO);
    else IF_KEYWORD("else", TOKEN_ELSE);
    else IF_KEYWORD("end", TOKEN_END);

    else IF_KEYWORD("for", TOKEN_FOR);
    else IF_KEYWORD("if", TOKEN_IF);
    else IF_KEYWORD("odd", TOKEN_ODD);
    else IF_KEYWORD("procedure", TOKEN_PROCEDURE);
    else IF_KEYWORD("program", TOKEN_PROGRAM);
    else IF_KEYWORD("then", TOKEN_THEN);
    else IF_KEYWORD("to", TOKEN_TO);
    else IF_KEYWORD("var", TOKEN_VAR);
    else IF_KEYWORD("while", TOKEN_WHILE);
    else
        put_token(TOKEN_IDENT);
}

static void NUMBER() {
    int line = rd_line();
    int col = rd_col();
    while (isdigit(rd_get())) {
        put_number(rd_get(), line, col);
        rd_next();
    }
    number_end();
    put_token(TOKEN_NUMBER);
}

static void START();

static void MULTIPLE_COMMENT() {
    while (rd_get() != '\0') {
        if (rd_get() == '*') {
            rd_next();
            if (rd_get() == ')') {
                rd_next();
                break;
            }
        }
        else {
            rd_next();
        }
    }
    START();
}

static void SINGLE_COMMENT() {
    while (rd_get() != '\0') {
        if (rd_get() == '\n') {
            rd_next();
            break;
        }
        else {
            rd_next();
        }
    }
    START();
}

#define ELSE_NEXT(ch, token) \
    else if (rd_get() == ch) { \
        rd_next(); \
        put_token(token); \
    }

static void START() {
    while (isspace(rd_get()))
        rd_next();

    if (rd_get() == '\0')
        return;

    if (isalpha(rd_get())) {
        NAME();
    }
    else if (isdigit(rd_get())) {
        NUMBER();
    }

    ELSE_NEXT('+', TOKEN_PLUS)
    ELSE_NEXT('-', TOKEN_MINUS)
    ELSE_NEXT('*', TOKEN_TIMES)
    else if (rd_get() == '/') {
        rd_next();
        if (rd_get() == '/') {
            rd_next();
            SINGLE_COMMENT();
        }
        else {
            put_token(TOKEN_DIVIDE);
        }
    }
    ELSE_NEXT('%', TOKEN_REMAINDER)

    ELSE_NEXT('=', TOKEN_EQ)
    else if (rd_get() == '<') {
        rd_next();
        if (rd_get() == '>') {
            rd_next();
            put_token(TOKEN_NE);
        }
        else if (rd_get() == '=') {
            rd_next();
            put_token(TOKEN_LE);
        }
        else {
            put_token(TOKEN_LT);
        }
    }
    else if (rd_get() == '>') {
        rd_next();
        if (rd_get() == '=') {
            rd_next();
            put_token(TOKEN_GE);
        }
        else {
            put_token(TOKEN_GT);
        }
    }

    else if (rd_get() == '(') {
        rd_next();
        if (rd_get() == '*') {
            rd_next();
            MULTIPLE_COMMENT();
        }
        else {
            put_token(TOKEN_LPARENT);
        }
    }
    ELSE_NEXT(')', TOKEN_RPARENT)
    ELSE_NEXT('[', TOKEN_LBRACKET)
    ELSE_NEXT(']', TOKEN_RBRACKET)

    ELSE_NEXT('.', TOKEN_PERIOD)
    ELSE_NEXT(',', TOKEN_COMMA)
    ELSE_NEXT(';', TOKEN_SEMICOLON)
    else if (rd_get() == ':') {
        rd_next();
        if (rd_get() != '=')
            error("Thieu dau = ", rd_line(), rd_col());
        rd_next();
        put_token(TOKEN_ASSIGN);
    }
    else {
        error("Khong nhan dien duoc ki tu", rd_line(), rd_col());
    }
}

void sc_next() {
    START();
    if (rd_get() == '\0')
        g_token = END_OF_TOKENS;
}

#define CASE(token, str) \
    case token: printf("%s\n", str); break

void sc_print(enum Token t) {
    switch (t) {
        case TOKEN_IDENT: printf("ident: %s\n", sc_name()); break;
        case TOKEN_NUMBER: printf("number: %d\n", sc_number()); break;

        CASE(TOKEN_BEGIN, "begin");
        CASE(TOKEN_CALL, "call");
        CASE(TOKEN_CONST, "const");
        CASE(TOKEN_DO, "do");
        CASE(TOKEN_ELSE, "else");
        CASE(TOKEN_END, "end");
        CASE(TOKEN_FOR, "for");
        CASE(TOKEN_IF, "if");
        CASE(TOKEN_ODD, "odd");
        CASE(TOKEN_PROCEDURE, "procedure");
        CASE(TOKEN_PROGRAM, "program");
        CASE(TOKEN_THEN, "then");
        CASE(TOKEN_TO, "to");
        CASE(TOKEN_VAR, "var");
        CASE(TOKEN_WHILE, "while");

        CASE(TOKEN_PLUS, "+");
        CASE(TOKEN_MINUS, "-");
        CASE(TOKEN_TIMES, "*");
        CASE(TOKEN_DIVIDE, "/");
        CASE(TOKEN_REMAINDER, "%");

        CASE(TOKEN_EQ, "=");
        CASE(TOKEN_NE, "<>");
        CASE(TOKEN_LT, "<");
        CASE(TOKEN_GT, ">");
        CASE(TOKEN_LE, "<=");
        CASE(TOKEN_GE, ">=");

        CASE(TOKEN_LPARENT, "(");
        CASE(TOKEN_RPARENT, ")");
        CASE(TOKEN_LBRACKET, "[");
        CASE(TOKEN_RBRACKET, "]");

        CASE(TOKEN_PERIOD, ".");
        CASE(TOKEN_COMMA, ",");
        CASE(TOKEN_SEMICOLON, ";");
        CASE(TOKEN_ASSIGN, ":=");

        case END_OF_TOKENS: break;
    }
}
