#include "scope.h"
#include <stack>
#include <iostream>
#include <cstdlib>

std::unique_ptr<Scope> g_root_scope;
std::stack<Scope *> g_scope_stack;

void error(const std::string& s) {
    std::cout << "Loi: ";
    std::cout << s << std::endl;
    std::exit(-1);
}

void scope_init() {
    g_root_scope = std::unique_ptr<Scope>{new Scope{}};
    g_root_scope->parent = nullptr;
    g_scope_stack.push(g_root_scope.get());
}

std::unique_ptr<Scope> scope_new() {
    auto ptr = std::unique_ptr<Scope>{new Scope{}};
    ptr->parent = scope_top();
    g_scope_stack.push(ptr.get());
    return ptr;
}

void scope_pop() {
    g_scope_stack.pop();
}

bool scope_empty() { return g_scope_stack.empty(); }

Scope *scope_top() {
    return g_scope_stack.top();
}

void scope_add(NameEntry&& entry) {
    Scope *current = scope_top();
    for (auto& e: current->names) {
        if (e.name == entry.name) 
            error("Trung ten: " + e.name);
    }
    scope_top()->names.push_back(std::move(entry));
}

bool scope_find(const std::string& name) {
    Scope *current = scope_top();
    while (current != nullptr) {
        for (auto& e: current->names)
            if (e.name == name)
                return true;
        current = current->parent;
    }
    return false;
}
