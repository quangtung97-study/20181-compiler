#ifndef READER_HPP
#define READER_HPP

#include <iostream>
#include <string>

void rd_set(std::istream& in);

int rd_get();
int rd_col();
int rd_line();

std::string rd_all();

void rd_next();

#endif
