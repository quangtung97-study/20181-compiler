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

struct VarType {
    bool is_array;
    bool is_reference;
    int array_size;
};

struct ProcType {
    std::vector<VarType> param_types;
};

enum ValueCategory {
    LVALUE,
    RVALUE
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

    // kind procedure
    ProcType proc_type;
    std::unique_ptr<Scope> proc_scope = nullptr;
};

struct Scope {
    std::string name;
    struct Scope *parent = nullptr;
    int mem_size = 0;
    std::vector<NameEntry> names;
};

void scope_init();

std::unique_ptr<Scope> scope_new();

void scope_pop();

Scope *scope_top();

void scope_add_var(const std::string& name, 
        bool is_reference = false,
        bool is_array = false, 
        int array_size = 0);

void scope_add_const(const std::string& name, int value);

void scope_add_proc(const std::string& name, int param_count,
        std::unique_ptr<Scope> scope);


NameEntry *scope_find(const std::string& name);

#endif
