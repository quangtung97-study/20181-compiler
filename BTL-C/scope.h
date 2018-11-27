#ifndef SCOPE_H
#define SCOPE_H

struct NameEntry {
    char name[16];
};

struct NameList {
    int size;
    int capacity;
    struct NameEntry *data;
};

struct Scope {
    struct Scope *parent;
    int mem_size;
};

struct Scope *scope_new();
void scope_delete(struct Scope *s);

#endif
