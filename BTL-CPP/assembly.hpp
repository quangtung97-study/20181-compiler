#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include "opcode.hpp"
#include <cstdio>

void as_init(FILE *file);

void as_save();

int as_pc();

void as_load_addr(int p, int q);

void as_load_value(int p, int q);

void as_load_const(int v);

void as_load_indirect();

void as_inc(int value);

void as_dec(int value);

void as_call(int depth, int addr);

void as_read_int();

void as_write_int();

void as_ret();

void as_halt();

#endif
