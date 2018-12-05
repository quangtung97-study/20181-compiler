#ifndef READER_HPP
#define READER_HPP

#include <string>
#include <cstdio>

void rd_set(FILE *file);

int rd_get();
int rd_col();
int rd_line();

void rd_reset();
bool rd_line(std::string& line);

void rd_next();

#endif
