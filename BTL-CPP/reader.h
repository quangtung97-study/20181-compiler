#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <string>

// not automatic closing the file
void rd_set(FILE *fp);

int rd_get();
int rd_col();
int rd_line();

std::string rd_all();

void rd_next();

#endif
