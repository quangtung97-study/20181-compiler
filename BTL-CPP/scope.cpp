#include "reader.hpp"
#include "scanner.hpp"
#include "scope.hpp"
#include <iostream>
#include <cstdlib>

static std::unique_ptr<Scope> g_root_scope;
static Scope *g_current;

void scope_init() {
    g_root_scope = std::unique_ptr<Scope>{new Scope{}};
    g_root_scope->parent = nullptr;
    g_current = g_root_scope.get();
}

static void scope_add(NameEntry&& entry) {
    Scope *current = scope_top();
    for (auto& e: current->names) {
        if (e.name == entry.name) 
            error("Trung ten: " + e.name);
    }
    for (auto& e: current->params) {
        if (e.name == entry.name) 
            error("Trung ten: " + e.name);
    }
    scope_top()->names.push_back(std::move(entry));
}

static void scope_add_proc(const std::string& name,
        std::unique_ptr<Scope> scope)
{
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_PROC;
    entry.proc_scope = std::move(scope);

    entry.proc_scope->name = name;

    scope_add(std::move(entry));
}


void scope_new(const std::string& name) {
    auto scope_ptr = std::unique_ptr<Scope>{new Scope{}};
    Scope *ptr = scope_ptr.get();
    scope_add_proc(name, std::move(scope_ptr));

    ptr->parent = scope_top();
    g_current = ptr;
}

void scope_pop() {
    g_current = g_current->parent;
}

Scope *scope_top() {
    return g_current;
}

static void scope_add_param(NameEntry&& entry) {
    Scope *current = scope_top();
    for (auto& e: current->names) {
        if (e.name == entry.name) 
            error("Trung ten: " + e.name);
    }
    for (auto& e: current->params) {
        if (e.name == entry.name) 
            error("Trung ten: " + e.name);
    }
    scope_top()->params.push_back(std::move(entry));
}

void scope_add_var(const std::string& name,
        enum VarType var_type, int array_size) 
{
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_VAR;
    entry.var_type = var_type;

    entry.array_size = array_size;

    entry.offset = g_current->mem_size;
    if (var_type == VAR_ARRAY) {
        if (array_size <= 0)
            error("So luong phan tu mang khong cho phep: " + name);
        g_current->mem_size += array_size;
    }
    else
        g_current->mem_size += 1;

    scope_add(std::move(entry));
}

void scope_add_param(const std::string& name, bool is_reference) 
{
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_VAR;

    if (is_reference)
        entry.var_type = VAR_REF;
    else 
        entry.var_type = VAR_INT;

    entry.array_size = 0;

    entry.offset = g_current->param_mem_size;
    g_current->param_mem_size += 1;

    scope_add_param(std::move(entry));
}

void scope_add_const(const std::string& name, int value) {
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_CONST;
    entry.const_value = value;

    scope_add(std::move(entry));
}

static FindResult find_result(
        NameEntry &e, Scope *scope, 
        int depth, bool is_param)
{
    FindResult result;
    result.ep = &e;
    result.is_param = is_param;
    result.depth = depth;
    result.mem_size = scope->mem_size;
    result.param_mem_size = scope->param_mem_size;
    return result;
}

static FindResult find_null_result() {
    FindResult result;
    result.ep = nullptr;
    return result;
}

FindResult scope_find(const std::string& name) {
    Scope *current = scope_top();
    int depth = 0;
    while (current != nullptr) {
        for (auto& e: current->names) 
            if (e.name == name)
                return find_result(e, current, depth, false);

        for (auto& e: current->params)
            if (e.name == name)
                return find_result(e, current, depth, true);

        current = current->parent;
        depth++;
    }
    return find_null_result();
}

int scope_mem_size() {
    return g_current->mem_size;
}

int scope_param_mem_size() {
    return g_current->param_mem_size;
}
