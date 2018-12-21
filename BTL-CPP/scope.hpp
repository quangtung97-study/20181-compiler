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
    int proc_addr;
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

void scope_new(const std::string& proc_name);

void scope_pop();

Scope *scope_top();

void scope_add_var(const std::string& name, 
        enum VarType var_type, int array_size = 0);

void scope_add_param(const std::string& name, bool is_reference);

void scope_add_const(const std::string& name, int value);

struct FindResult {
    NameEntry *ep;
    bool is_param;
    int mem_size;
    int param_mem_size;
    int depth;
};

FindResult scope_find(const std::string& name);

int scope_mem_size();

int scope_param_mem_size();

#endif
