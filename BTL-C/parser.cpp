#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "scope.h"
#include <cstdlib>
#include <iostream>

void ps_init(FILE *fp) {
    sc_init(fp);
    scope_init();
}

static void error(const std::string& s) {
    std::cout << rd_all() << std::endl;
    std::cout << "Loi: " << s << std::endl;
    std::cout << "Tai dong " << sc_line() 
        << ", cot " << sc_col() << std::endl;
    std::exit(-1);
}

static void NEXT(enum Token token, const char *error_msg) {
    if (sc_get() != token)
        error(error_msg);
    sc_next();
}

static void CHECK(enum Token token, const char *error_msg) {
    if (sc_get() != token)
        error(error_msg);
}

static void EXPR();

static void FACTOR() {
    if (sc_get() == TOKEN_IDENT) {
        sc_next();
        if (sc_get() == TOKEN_LBRACKET) {
            sc_next();
            EXPR();
            NEXT(TOKEN_RBRACKET, "Thieu \"]\"");
        }
    }
    else if (sc_get() == TOKEN_NUMBER) {
        sc_next();
    }
    else if (sc_get() == TOKEN_LPARENT) {
        sc_next();
        EXPR();
        NEXT(TOKEN_RPARENT, "Thieu \")\"");
    }
    else {
        error("Thieu toan hang");
    }
}

static void TERM() {
    FACTOR();
term_loop:
    if (sc_get() == TOKEN_TIMES) {
        sc_next();
        FACTOR();
        goto term_loop;
    }
    if (sc_get() == TOKEN_REMAINDER) {
        sc_next();
        FACTOR();
        goto term_loop;
    }
    if (sc_get() == TOKEN_DIVIDE) {
        sc_next();
        FACTOR();
        goto term_loop;
    }
}

static void EXPR() {
    if (sc_get() == TOKEN_PLUS) {
        sc_next();
    }
    if (sc_get() == TOKEN_MINUS) {
        sc_next();
    }
    TERM();
expr_loop:
    if (sc_get() == TOKEN_PLUS) {
        sc_next();
        TERM();
        goto expr_loop;
    }
    if (sc_get() == TOKEN_MINUS) {
        sc_next();
        TERM();
        goto expr_loop;
    }
}

static void CONDITION() {
    if (sc_get() == TOKEN_ODD) {
        sc_next();
        EXPR();
    }
    else {
        EXPR();
        if (sc_get() == TOKEN_EQ)
            sc_next();
        else if (sc_get() == TOKEN_GT)
            sc_next();
        else if (sc_get() == TOKEN_GE)
            sc_next();
        else if (sc_get() == TOKEN_LT)
            sc_next();
        else if (sc_get() == TOKEN_LE)
            sc_next();
        else if (sc_get() == TOKEN_NE) 
            sc_next();
        else 
            error("Thieu mot toan tu so sanh");
        EXPR();
    }
}

static void STATEMENT();
static void BEGIN();

static void ASSIGN_STMT() {
    if (sc_get() == TOKEN_LBRACKET) {
        sc_next();
        EXPR();
        NEXT(TOKEN_RBRACKET, "Thieu \"]\"");
    }

    NEXT(TOKEN_ASSIGN, "Thieu dau gan := ");
    EXPR();
}

static void CALL_STMT() {
    NEXT(TOKEN_IDENT, "Thieu ten thu tuc duoc goi");
    if (sc_get() == TOKEN_LPARENT) {
        sc_next();
next_arg:
        EXPR();
        if (sc_get() == TOKEN_COMMA) {
            sc_next();
            goto next_arg;
        }
        NEXT(TOKEN_RPARENT, "Thieu dau \")\"");
    }
}

static void IF_STMT() {
    CONDITION();
    NEXT(TOKEN_THEN, "Thieu THEN");
    STATEMENT();
    if (sc_get() == TOKEN_ELSE) {
        sc_next();
        STATEMENT();
    }
}

static void WHILE_STMT() {
    CONDITION();
    NEXT(TOKEN_DO, "Thieu DO");
    STATEMENT();
}

static void FOR_STMT() {
    NEXT(TOKEN_IDENT, "Thieu ten bien chay FOR");
    NEXT(TOKEN_ASSIGN, "Thieu phep gan := ");
    EXPR();
    NEXT(TOKEN_TO, "Thieu TO");
    EXPR();
    NEXT(TOKEN_DO, "Thieu DO");
    STATEMENT();
}

static void STATEMENT() {
    if (sc_get() == TOKEN_IDENT) {
        std::string name = sc_name();
        if (scope_find(name) == false)
            error("Ten chua duoc khai bao: " + name);
        sc_next();
        ASSIGN_STMT();
    }
    else if (sc_get() == TOKEN_CALL) {
        sc_next();
        CALL_STMT();
    }
    else if (sc_get() == TOKEN_IF) {
        sc_next();
        IF_STMT();
    }
    else if (sc_get() == TOKEN_WHILE) {
        sc_next();
        WHILE_STMT();
    }
    else if (sc_get() == TOKEN_FOR) {
        sc_next();
        FOR_STMT();
    }
    else if (sc_get() == TOKEN_BEGIN) {
        sc_next();
        BEGIN();
    }
}

static void BLOCK();

static void PROCEDURE() {
    CHECK(TOKEN_IDENT, "Thieu ten thu tuc");
    std::string name = sc_name();
    sc_next();

    auto scope_ptr = scope_new();

    if (sc_get() == TOKEN_LPARENT) {
        sc_next();
loop_args:
        if (sc_get() == TOKEN_VAR)
            sc_next();
        NEXT(TOKEN_IDENT, "Thieu ten tham so cua thu tuc");
        if (sc_get() == TOKEN_SEMICOLON) {
            sc_next();
            goto loop_args;
        }

        NEXT(TOKEN_RPARENT, "Thieu dau \")\"");
    }

    NEXT(TOKEN_SEMICOLON, "Thieu dau ; sau khai bao cac tham so thu tuc");
    BLOCK();
    NEXT(TOKEN_SEMICOLON, "Thieu dau ; ket thuc thu tuc");

    scope_pop();

    NameEntry entry;
    entry.name = std::move(name);
    entry.type = TYPE_PROC;
    entry.proc_scope = std::move(scope_ptr);
    scope_add(std::move(entry));
}

static void BEGIN() {
    STATEMENT();
    while (sc_get() == TOKEN_SEMICOLON) {
        sc_next();
        STATEMENT();
    }
    NEXT(TOKEN_END, "Thieu ; hoac END");
}

static void VAR() {
    CHECK(TOKEN_IDENT, "Thieu ten bien");
    std::string name = sc_name();
    sc_next();

    if (sc_get() == TOKEN_LBRACKET) {
        sc_next();
        NEXT(TOKEN_NUMBER, "Thieu chi so mang");
        NEXT(TOKEN_RBRACKET, "Thieu dau \"]\"");
    }

    NameEntry entry;
    entry.name = std::move(name);
    entry.type = TYPE_INT;
    entry.offset = scope_top()->mem_size;
    scope_add(std::move(entry));
    scope_top()->mem_size += 4;

    if (sc_get() == TOKEN_COMMA) {
        sc_next();
        VAR();
    }
    else if (sc_get() == TOKEN_SEMICOLON) {
        sc_next();
    }
    else {
        error("Thieu dau cham phay ket thuc khai bao bien");
    }
}

static void CONST() {
    CHECK(TOKEN_IDENT, "Thieu ten cho hang");
    std::string const_name = sc_name();
    sc_next();

    NEXT(TOKEN_EQ, "Thieu dau =");

    CHECK(TOKEN_NUMBER, "Thieu so nguyen");
    int const_num = sc_number();
    sc_next();

    NameEntry entry;
    entry.name = std::move(const_name);
    entry.type = TYPE_CONST;
    entry.const_value = const_num;
    scope_add(std::move(entry));

    if (sc_get() == TOKEN_COMMA) {
        sc_next();
        CONST();
    }
    else if (sc_get() == TOKEN_SEMICOLON) {
        sc_next();
    }
    else {
        error("Thieu dau cham phay ket thuc khai bao hang");
    }
}

static void BLOCK() {
    if (sc_get() == TOKEN_CONST) {
        sc_next();
        CONST();
    }

    if (sc_get() == TOKEN_VAR) {
        sc_next();
        VAR();
    }

    while (sc_get() == TOKEN_PROCEDURE) {
        sc_next();
        PROCEDURE();
    }
    
    if (sc_get() == TOKEN_BEGIN) {
        sc_next();
        BEGIN();
    }
    else {
        error("Thieu BEGIN");
    }
}

static void PROGRAM() {
    NEXT(TOKEN_PROGRAM, "Thieu tu khoa PROGRAM");
    NEXT(TOKEN_IDENT, "Thieu ten chuong trinh");
    NEXT(TOKEN_SEMICOLON, "Thieu dau cham phay");
    BLOCK();
    NEXT(TOKEN_PERIOD, "Thieu dau cham");
}

void ps_parse() {
    PROGRAM();
}