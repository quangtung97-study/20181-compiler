#include "reader.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "scope.hpp"
#include <sstream>

void ps_init(std::istream& in) {
    sc_init(in);
    scope_init();
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

static NameEntry *CHECK_VAR() {
    std::string name = sc_name();
    auto ep = scope_find(name);
    if (ep == nullptr) {
        error("Ten chua duoc khai bao: " + name);
    }
    else if (ep->kind != KIND_VAR) {
        error("Ten khong phai la bien: " + name);
    }
    return ep;
}

static NameEntry *CHECK_VAR_CONST() {
    std::string name = sc_name();
    auto ep = scope_find(name);
    if (ep == nullptr) {
        error("Ten chua duoc khai bao: " + name);
    }
    if (ep->kind != KIND_VAR && ep->kind != KIND_CONST) {
        error("Ten khong phai bien hoac hang: " + name);
    }
    return ep;
}

static void CHECK_IS_ARRAY(NameEntry *e) {
    if (e->kind == KIND_VAR && e->var_type == VAR_ARRAY)
        return;
    error("Khong phai la bien mang: " + e->name);
}

static void CHECK_NOT_ARRAY(NameEntry *e) {
    if (e->kind == KIND_CONST)
        return;
    if (e->kind == KIND_VAR && e->var_type != VAR_ARRAY)
        return;
    error("Thieu [] truy suat chi so mang");
}

static NameEntry *CHECK_PROC() {
    std::string name = sc_name();
    auto ep = scope_find(name);
    if (ep == nullptr) {
        error("Ten chua duoc khai bao: " + name);
    }
    else if (ep->kind != KIND_PROC) {
        error("Ten khong phai la thu tuc: " + name);
    }
    return ep;
}

static ValueCategory EXPR();

static ValueCategory FACTOR() {
    ValueCategory category = LVALUE;

    if (sc_get() == TOKEN_IDENT) {
        auto ep = CHECK_VAR_CONST();
        sc_next();

        if (sc_get() == TOKEN_LBRACKET) {
            CHECK_IS_ARRAY(ep);
            sc_next();

            EXPR();
            NEXT(TOKEN_RBRACKET, "Thieu \"]\"");
        }
        else {
            CHECK_NOT_ARRAY(ep);
        }

        if (ep->kind == KIND_VAR)
            category = LVALUE;
        else 
            category = RVALUE;
    }
    else if (sc_get() == TOKEN_NUMBER) {
        sc_next();
        category = RVALUE;
    }
    else if (sc_get() == TOKEN_LPARENT) {
        sc_next();
        category = EXPR();
        NEXT(TOKEN_RPARENT, "Thieu \")\"");
    }
    else {
        error("Thieu mot Factor");
    }

    return category;
}

static ValueCategory TERM() {
    auto category = FACTOR();
term_loop:
    if (sc_get() == TOKEN_TIMES) {
        sc_next();
        category = RVALUE;
        FACTOR();
        goto term_loop;
    }
    if (sc_get() == TOKEN_REMAINDER) {
        sc_next();
        category = RVALUE;
        FACTOR();
        goto term_loop;
    }
    if (sc_get() == TOKEN_DIVIDE) {
        sc_next();
        category = RVALUE;
        FACTOR();
        goto term_loop;
    }

    return category;
}

static ValueCategory EXPR() {
    ValueCategory category = LVALUE;
    if (sc_get() == TOKEN_PLUS) {
        sc_next();
        category = RVALUE;
    }
    if (sc_get() == TOKEN_MINUS) {
        sc_next();
        category = RVALUE;
    }

    auto term_category = TERM();
    category = term_category == LVALUE ? category : RVALUE;

expr_loop:
    if (sc_get() == TOKEN_PLUS) {
        sc_next();
        category = RVALUE;
        TERM();
        goto expr_loop;
    }
    if (sc_get() == TOKEN_MINUS) {
        sc_next();
        category = RVALUE;
        TERM();
        goto expr_loop;
    }

    return category;
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

static void ASSIGN_STMT(NameEntry *e) {
    if (sc_get() == TOKEN_LBRACKET) {
        CHECK_IS_ARRAY(e);
        sc_next();

        EXPR();
        NEXT(TOKEN_RBRACKET, "Thieu \"]\"");
    }
    else {
        CHECK_NOT_ARRAY(e);
    }

    NEXT(TOKEN_ASSIGN, "Thieu dau gan := ");
    EXPR();
}

static void CALL_STMT() {
    CHECK(TOKEN_IDENT, "Thieu ten thu tuc duoc goi");
    auto ep = CHECK_PROC();
    sc_next();

    int arg_count = 0;
    int max_arg_count = ep->proc_scope->params.size();

    if (sc_get() == TOKEN_LPARENT) {
        sc_next();
next_arg:
        if (arg_count == max_arg_count) {
            std::ostringstream ss;
            ss << "\n\tThua so doi so goi ham" << std::endl;
            ss << "\tChi can " << max_arg_count << " doi so";
            error(ss.str());
        }

        auto category = EXPR();
        const auto& e = ep->proc_scope->params[arg_count];

        if (e.var_type == VAR_REF && category == RVALUE) {
            std::ostringstream ss;
            ss << "\n\tThu tuc: " << ep->name << std::endl;
            ss << "\tVi tri doi so thu: " << arg_count + 1 << std::endl;
            ss << "\tCan mot tham bien nhung da cung cap mot gia tri";
            error(ss.str());
        }

        arg_count++;

        if (sc_get() == TOKEN_COMMA) {
            sc_next();
            goto next_arg;
        }
        NEXT(TOKEN_RPARENT, "Thieu dau \")\"");
    }

    if (arg_count < max_arg_count) {
        std::ostringstream ss;
        ss << "\n\tThieu so doi so goi ham" << std::endl;
        ss << "\tCan " << max_arg_count << " gia tri" << std::endl;
        ss << "\tNhung da cung cap " << arg_count << " gia tri";
        error(ss.str());
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
    CHECK(TOKEN_IDENT, "Thieu ten bien chay FOR");
    auto ep = CHECK_VAR();
    if (ep->var_type == VAR_ARRAY)
        error("Khong can mot bien mang");
    sc_next();

    NEXT(TOKEN_ASSIGN, "Thieu phep gan := ");
    EXPR();
    NEXT(TOKEN_TO, "Thieu TO");
    EXPR();
    NEXT(TOKEN_DO, "Thieu DO");
    STATEMENT();
}

static void STATEMENT() {
    if (sc_get() == TOKEN_IDENT) {
        auto e = CHECK_VAR();
        sc_next();
        ASSIGN_STMT(e);
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
        bool is_reference = false;
        if (sc_get() == TOKEN_VAR) {
            is_reference = true;
            sc_next();
        }

        CHECK(TOKEN_IDENT, "Thieu ten tham so cua thu tuc");
        std::string param_name = sc_name();
        sc_next();

        scope_add_param(param_name, is_reference);

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
    scope_add_proc(name, std::move(scope_ptr));
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

    VarType var_type = VAR_INT;
    int array_size = 0;

    if (sc_get() == TOKEN_LBRACKET) {
        sc_next();
        var_type = VAR_ARRAY;

        CHECK(TOKEN_NUMBER, "Thieu chi so mang");
        array_size = sc_number();
        sc_next();

        NEXT(TOKEN_RBRACKET, "Thieu dau \"]\"");
    }

    scope_add_var(name, var_type, array_size);

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
    std::string name = sc_name();
    sc_next();

    NEXT(TOKEN_EQ, "Thieu dau =");

    CHECK(TOKEN_NUMBER, "Thieu so nguyen");
    int const_value = sc_number();
    sc_next();

    scope_add_const(name, const_value);

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
