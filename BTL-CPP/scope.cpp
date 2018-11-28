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

std::unique_ptr<Scope> scope_new() {
    auto ptr = std::unique_ptr<Scope>{new Scope{}};
    ptr->parent = scope_top();
    g_current = ptr.get();
    return ptr;
}

void scope_pop() {
    g_current = g_current->parent;
}

Scope *scope_top() {
    return g_current;
}

static void scope_add(NameEntry&& entry) {
    Scope *current = scope_top();
    for (auto& e: current->names) {
        if (e.name == entry.name) 
            error("Trung ten: " + e.name);
    }
    scope_top()->names.push_back(std::move(entry));
}

void scope_add_var(const std::string& name,
        bool is_reference, 
        bool is_array,
        int array_size
        ) 
{
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_VAR;

    entry.var_type.is_reference = is_reference;
    entry.var_type.is_array = is_array;
    entry.var_type.array_size = array_size;

    entry.offset = g_current->mem_size;
    if (is_array) 
        g_current->mem_size += array_size * 4;
    else
        g_current->mem_size += 4;

    scope_add(std::move(entry));
}

void scope_add_const(const std::string& name, int value) {
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_CONST;
    entry.const_value = value;

    scope_add(std::move(entry));
}

void scope_add_proc(const std::string& name, int param_count,
        std::unique_ptr<Scope> scope)
{
    NameEntry entry;
    entry.name = name;
    entry.kind = KIND_PROC;
    entry.proc_scope = std::move(scope);

    Scope *child_scope = entry.proc_scope.get();
    child_scope->name = name;
    for (int i = 0; i < param_count; i++) {
        auto& e = child_scope->names[i];
        entry.proc_type.param_types.push_back(e.var_type);
    }

    scope_add(std::move(entry));
}


NameEntry *scope_find(const std::string& name) {
    Scope *current = scope_top();
    while (current != nullptr) {
        for (auto& e: current->names)
            if (e.name == name)
                return &e;
        current = current->parent;
    }
    return nullptr;
}
