#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <string>
#include <vector>
#include <memory>

enum Kind {
    KIND_VAR,
    KIND_PROC,
    KIND_CONST,
};

enum VarType {
    VAR_INT,
    VAR_REF, 
    VAR_ARRAY,
};

enum ValueCategory {
    LVALUE,
    RVALUE,
};

struct Scope;

struct NameEntry {
    std::string name;
    enum Kind kind;

    // kind const
    int const_value;

    // kind variable
    VarType var_type;
    int offset = -1;
    int array_size;

    // kind procedure
    std::unique_ptr<Scope> proc_scope = nullptr;
};

struct Scope {
    std::string name;
    struct Scope *parent = nullptr;

    std::vector<NameEntry> params;
    int param_mem_size = 0;

    std::vector<NameEntry> names;
    int mem_size = 0;
};

void scope_init();

std::unique_ptr<Scope> scope_new();

void scope_pop();

Scope *scope_top();

void scope_add_var(const std::string& name, 
        enum VarType var_type, int array_size = 0);

void scope_add_param(const std::string& name, bool is_reference);

void scope_add_const(const std::string& name, int value);

void scope_add_proc(const std::string& name,
        std::unique_ptr<Scope> scope);


NameEntry *scope_find(const std::string& name);

#endif
