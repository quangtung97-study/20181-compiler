#include "reader.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "scope.hpp"
#include "assembly.hpp"
#include <sstream>

void ps_init(FILE *file) {
    sc_init(file);
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

static FindResult CHECK_VAR() {
    std::string name = sc_name();
    auto find = scope_find(name);
    if (find.ep == nullptr) {
        error("Ten chua duoc khai bao: " + name);
    }
    else if (find.ep->kind != KIND_VAR) {
        error("Ten khong phai la bien: " + name);
    }
    return find;
}

static FindResult CHECK_VAR_CONST() {
    std::string name = sc_name();
    auto find = scope_find(name);
    if (find.ep == nullptr) {
        error("Ten chua duoc khai bao: " + name);
    }
    if (find.ep->kind != KIND_VAR && find.ep->kind != KIND_CONST) {
        error("Ten khong phai bien hoac hang: " + name);
    }
    return find;
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

static FindResult CHECK_PROC() {
    std::string name = sc_name();
    auto find = scope_find(name);
    if (find.ep == nullptr) {
        error("Ten chua duoc khai bao: " + name);
    }
    else if (find.ep->kind != KIND_PROC) {
        error("Ten khong phai la thu tuc: " + name);
    }
    return find;
}

static ValueCategory EXPR();

static void load_variable(FindResult find) {
    int offset;
    if (find.is_param) 
        offset = CALL_FRAME_SIZE + find.ep->offset;
    else 
        offset = CALL_FRAME_SIZE + find.param_mem_size + find.ep->offset;

    if (find.ep->var_type == VAR_REF)
        as_load_value(find.depth, offset);
    else
        as_load_addr(find.depth, offset);
}

static ValueCategory FACTOR() {
    ValueCategory category;

    if (sc_get() == TOKEN_IDENT) {
        auto find = CHECK_VAR_CONST();
        sc_next();

        if (sc_get() == TOKEN_LBRACKET) {
            category = LVALUE;

            CHECK_IS_ARRAY(find.ep);
            load_variable(find);
            sc_next();

            int category = EXPR();
            if (category == LVALUE)
                as_load_indirect();

            NEXT(TOKEN_RBRACKET, "Thieu \"]\"");
            as_add();
        }
        else {
            CHECK_NOT_ARRAY(find.ep);
            if (find.ep->kind == KIND_CONST) {
                category = RVALUE;
                as_load_const(find.ep->const_value);
            }
            else {
                category = LVALUE;
                load_variable(find);
            }
        }
    }
    else if (sc_get() == TOKEN_NUMBER) {
        category = RVALUE;
        as_load_const(sc_number());
        sc_next();
    }
    else if (sc_get() == TOKEN_LPARENT) {
        sc_next();
        category = EXPR();
        NEXT(TOKEN_RPARENT, "Thieu \")\"");
    }
    else {
        category = LVALUE;
        error("Thieu mot Factor");
    }

    return category;
}

static ValueCategory TERM() {
    auto category = FACTOR();
term_loop:
    if (sc_get() == TOKEN_TIMES) {
        sc_next();
        
        if (category == LVALUE)
            as_load_indirect();

        category = FACTOR();
        if (category == LVALUE)
            as_load_indirect();

        as_mul();

        category = RVALUE;
        goto term_loop;
    }
    if (sc_get() == TOKEN_REMAINDER) {
        sc_next();

        if (category == LVALUE)
            as_load_indirect();

        category = FACTOR();
        if (category == LVALUE)
            as_load_indirect();

        as_mod();

        category = RVALUE;
        goto term_loop;
    }
    if (sc_get() == TOKEN_DIVIDE) {
        sc_next();

        if (category == LVALUE)
            as_load_indirect();
        
        category = FACTOR();
        if (category == LVALUE)
            as_load_indirect();

        as_div();

        category = RVALUE;
        goto term_loop;
    }

    return category;
}

static ValueCategory EXPR() {
    ValueCategory category;
    bool negate = false;
    bool lvalue_to_rvalue = false;

    if (sc_get() == TOKEN_PLUS) {
        sc_next();
        lvalue_to_rvalue = true;
    }
    if (sc_get() == TOKEN_MINUS) {
        sc_next();
        lvalue_to_rvalue = true;
        negate = true;
    }

    category = TERM();
    if (lvalue_to_rvalue) {
        if (category == LVALUE)
            as_load_indirect();

        category = RVALUE;

        if (negate)
            as_neg();
    }

expr_loop:
    if (sc_get() == TOKEN_PLUS) {
        sc_next();

        if (category == LVALUE)
            as_load_indirect();

        category = TERM();
        if (category == LVALUE)
            as_load_indirect();

        category = RVALUE;
        as_add();
        goto expr_loop;
    }
    if (sc_get() == TOKEN_MINUS) {
        sc_next();

        if (category == LVALUE)
            as_load_indirect();

        category = TERM();
        if (category == LVALUE)
            as_load_indirect();

        category = RVALUE;
        as_sub();
        goto expr_loop;
    }

    return category;
}

static void CONDITION() {
    if (sc_get() == TOKEN_ODD) {
        sc_next();
        auto category = EXPR();
        if (category == LVALUE)
            as_load_indirect();
        as_odd();
    }
    else {
        auto category = EXPR();
        if (category == LVALUE)
            as_load_indirect();

        enum Token token = TOKEN_END;
        if (sc_get() == TOKEN_EQ) {
            token = TOKEN_EQ;
            sc_next();
        }
        else if (sc_get() == TOKEN_GT) {
            token = TOKEN_GT;
            sc_next();
        }
        else if (sc_get() == TOKEN_GE) {
            token = TOKEN_GE;
            sc_next();
        }
        else if (sc_get() == TOKEN_LT) {
            token = TOKEN_LT;
            sc_next();
        }
        else if (sc_get() == TOKEN_LE) {
            token = TOKEN_LE;
            sc_next();
        }
        else if (sc_get() == TOKEN_NE) {
            token = TOKEN_NE;
            sc_next();
        }
        else {
            error("Thieu mot toan tu so sanh");
        }

        category = EXPR();
        if (category == LVALUE)
            as_load_indirect();

        if (token == TOKEN_EQ) 
            as_eq();
        else if (token == TOKEN_GT) 
            as_gt();
        else if (token == TOKEN_GE) 
            as_ge();
        else if (token == TOKEN_LT) 
            as_lt();
        else if (token == TOKEN_LE) 
            as_le();
        else if (token == TOKEN_NE) 
            as_ne();
    }
}

static void STATEMENT();
static void BEGIN();

static void ASSIGN_STMT(FindResult find) {
    if (sc_get() == TOKEN_LBRACKET) {
        CHECK_IS_ARRAY(find.ep);
        load_variable(find);
        sc_next();

        auto category = EXPR();
        if (category == LVALUE)
            as_load_indirect();

        as_add();

        NEXT(TOKEN_RBRACKET, "Thieu \"]\"");
    }
    else {
        CHECK_NOT_ARRAY(find.ep);
        load_variable(find);
    }

    NEXT(TOKEN_ASSIGN, "Thieu dau gan := ");
    auto category = EXPR();
    if (category == LVALUE)
        as_load_indirect();

    as_store();
}

static bool is_special_proc(const std::string& name) {
    if (name == "readi")
        return true;
    if (name == "writei")
        return true;
    if (name == "writeln")
        return true;
    return false;
}

static void CALL_STMT() {
    CHECK(TOKEN_IDENT, "Thieu ten thu tuc duoc goi");
    const auto find = CHECK_PROC();
    sc_next();

    bool is_special = is_special_proc(find.ep->name);
    int arg_count = 0;
    int max_arg_count = find.ep->proc_scope->params.size();

    if (!is_special) 
        as_inc(CALL_FRAME_SIZE);

    if (sc_get() == TOKEN_LPARENT) {
        sc_next();
next_arg:
        if (arg_count == max_arg_count) {
            if (arg_count == 0) {
                std::ostringstream ss;
                ss << "Khong can () cho goi thu tuc: " 
                    << find.ep->name << std::endl;
                error(ss.str());
            }
            else {
                std::ostringstream ss;
                ss << "\n\tThua so doi so goi ham" << std::endl;
                ss << "\tChi can " << max_arg_count << " doi so";
                error(ss.str());
            }
        }

        auto category = EXPR();
        const auto& e = find.ep->proc_scope->params[arg_count];

        if (e.var_type == VAR_REF && category == RVALUE) {
            std::ostringstream ss;
            ss << "\n\tThu tuc: " << find.ep->name << std::endl;
            ss << "\tVi tri doi so thu: " << arg_count + 1 << std::endl;
            ss << "\tCan mot tham bien nhung da cung cap mot gia tri";
            error(ss.str());
        }

        if (e.var_type == VAR_INT && category == LVALUE)
            as_load_indirect();

        arg_count++;

        if (sc_get() == TOKEN_COMMA) {
            sc_next();
            goto next_arg;
        }
        NEXT(TOKEN_RPARENT, "Thieu dau \")\"");
    }

    if (arg_count < max_arg_count) {
        std::ostringstream ss;
        ss << "\n\tThieu so doi so goi thu tuc: " 
            << find.ep->name << std::endl;
        ss << "\tCan " << max_arg_count << " gia tri" << std::endl;
        ss << "\tNhung da cung cap " << arg_count << " gia tri";
        error(ss.str());
    }

    if (is_special) {
        auto& name = find.ep->name;
        if (name == "readi")
            as_read_int();
        else if (name == "writei")
            as_write_int();
        else 
            as_write_line();
    }
    else {
        as_dec(CALL_FRAME_SIZE + find.ep->proc_scope->param_mem_size);
        as_call(find.depth, find.ep->proc_scope->proc_addr);
    }
}

static void IF_STMT() {
    CONDITION();
    int jmp_index = as_false_jmp();

    NEXT(TOKEN_THEN, "Thieu THEN");
    STATEMENT();
    if (sc_get() == TOKEN_ELSE) {
        sc_next();
        int end_else_index = as_jmp();
        as_set_jmp_addr(jmp_index, as_code_addr());
        STATEMENT();
        as_set_jmp_addr(end_else_index, as_code_addr());
    }
    else {
        as_set_jmp_addr(jmp_index, as_code_addr());
    }
}

static void WHILE_STMT() {
    int begin_addr = as_code_addr();
    CONDITION();
    int jmp_index = as_false_jmp();
    NEXT(TOKEN_DO, "Thieu DO");
    STATEMENT();
    int jmp_index_loop = as_jmp();
    as_set_jmp_addr(jmp_index_loop, begin_addr);
    as_set_jmp_addr(jmp_index, as_code_addr());
}

static void FOR_STMT() {
    CHECK(TOKEN_IDENT, "Thieu ten bien chay FOR");
    auto find = CHECK_VAR();
    if (find.ep->var_type == VAR_ARRAY)
        error("Khong can mot bien mang");
    sc_next();

    load_variable(find);

    NEXT(TOKEN_ASSIGN, "Thieu phep gan := ");

    auto category = EXPR();
    if (category == LVALUE)
        as_load_indirect();

    as_store();

    int begin_addr = as_code_addr();
    NEXT(TOKEN_TO, "Thieu TO");

    load_variable(find);
    as_load_indirect();
    category = EXPR();
    if (category == LVALUE)
        as_load_indirect();
    as_le();

    int jmp_index = as_false_jmp();

    NEXT(TOKEN_DO, "Thieu DO");
    STATEMENT();
    
    load_variable(find);
    load_variable(find);
    as_load_indirect();
    as_load_const(1);
    as_add();
    as_store();

    int jmp_index_loop = as_jmp();
    as_set_jmp_addr(jmp_index_loop, begin_addr);
    as_set_jmp_addr(jmp_index, as_code_addr());
}

static void STATEMENT() {
    if (sc_get() == TOKEN_IDENT) {
        auto find = CHECK_VAR();
        sc_next();
        ASSIGN_STMT(find);
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

    scope_new(name);

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
    as_ret();
}

typedef const Token *Iterator;
static Iterator find_token(Iterator first, 
        Iterator last, Token token) 
{
    for (;first != last; ++first)
        if (*first == token)
            return first;
    return last;
}

static void BEGIN() {
    STATEMENT();
    while (sc_get() == TOKEN_SEMICOLON) {
        sc_next();
        STATEMENT();
    }
    if (sc_get() == TOKEN_END) {
        sc_next();
        return;
    }
    
    Token tokens[] = {
        TOKEN_IDENT, TOKEN_CALL, TOKEN_BEGIN, 
        TOKEN_IF, TOKEN_WHILE, TOKEN_FOR
    };

    auto find_it = find_token(
            std::begin(tokens), std::end(tokens), sc_get());
    if (find_it != std::end(tokens))
        error("Thieu ;");
    else
        error("Thieu END");
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

    scope_top()->proc_addr = as_code_addr();
    as_inc(CALL_FRAME_SIZE + 
            scope_mem_size() + scope_param_mem_size());
    
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

    // readi
    scope_new("readi");
    auto find = scope_find("readi");
    scope_add_param("n", true);
    scope_pop();

    // writei
    scope_new("writei");
    find = scope_find("writei");
    scope_add_param("n", false);
    scope_pop();

    // writeln
    scope_new("writeln");
    scope_pop();
    
    BLOCK();
    NEXT(TOKEN_PERIOD, "Thieu dau cham");
    as_halt();
    as_set_main(scope_top()->proc_addr);
}

void ps_parse() {
    PROGRAM();
}
