#ifndef READER_H
#define READER_H

#include <stdio.h>

// not automatic closing the file
void rd_set(FILE *fp);

int rd_get();
int rd_col();
int rd_line();

// need to free()
char *rd_all();

void rd_next();

#endif
