#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include "opcode.hpp"
#include <cstdio>

void as_init();

void as_set_main(int addr);

void as_save(FILE *file);

int as_code_addr();

void as_load_addr(int p, int q);

void as_load_value(int p, int q);

void as_load_const(int v);

void as_load_indirect();

void as_inc(int value);

void as_dec(int value);

void as_store();

void as_call(int depth, int addr);

void as_ret();

void as_halt();

void as_read_int();

void as_write_int();

void as_add();

void as_sub();

void as_mul();

void as_div();

void as_mod();

void as_neg();

void as_odd();

void as_eq();

void as_gt();

void as_ge();

void as_lt();

void as_le();

void as_ne();

#endif
