#ifndef SCOPE_H
#define SCOPE_H

#include <string>
#include <vector>
#include <memory>

enum Type {
    TYPE_INT,
    TYPE_PROC,
    TYPE_CONST,
};

struct Scope;

struct NameEntry {
    std::string name;
    enum Type type;

    int const_value = -1;
    int offset = -1;
    std::unique_ptr<Scope> proc_scope = nullptr;
};

struct Scope {
    struct Scope *parent = nullptr;
    int mem_size = 0;
    std::vector<NameEntry> names;
};

void scope_init();

std::unique_ptr<Scope> scope_new();

void scope_pop();

bool scope_empty();

Scope *scope_top();

void scope_add(NameEntry&& entry);

bool scope_find(const std::string& name);


#endif
