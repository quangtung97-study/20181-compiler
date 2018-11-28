#include "reader.hpp"
#include <algorithm>

#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0

static std::istream *g_in;
static char g_buff[BUFF_SIZE];
static int g_buff_index;
static int g_buff_end;
static int g_going_newline;
static int g_col;
static int g_line;

static void init(std::istream& in) {
    g_in = &in;
    g_buff_index = 0;
    g_buff_end = 0;
    g_going_newline = TRUE;
    g_col = 1;
    g_line = 0;
}

void rd_set(std::istream& in) {
    init(in);
    rd_next();
}

int rd_get() {
    return g_buff[g_buff_index];
}

void rd_next() {
    if (g_buff_end == g_buff_index) {
        g_in->read(g_buff, BUFF_SIZE);
        auto n = g_in->gcount();
        if (n == 0) {
            g_buff_end = 1;
            g_buff_index = 0;
            g_buff[0] = '\0';
        }
        else {
            g_buff_end = n;
            g_buff_index = 0;
        }
    }
    else {
        g_buff_index++;
    }

    if (g_going_newline) {
        g_col = 1;
        g_line++;
        g_going_newline = FALSE;
    }
    else {
        g_col++;
    }

    if (rd_get() == '\n')
        g_going_newline = TRUE;
}

int rd_col() { return g_col; }

int rd_line() { return g_line; }

std::string rd_all() {
    std::string result;
    g_in->clear();
    g_in->seekg(0, std::ios::beg);

    size_t count;
    do {
        g_in->read(g_buff, BUFF_SIZE);
        count = g_in->gcount();
        std::copy(g_buff, g_buff + count, 
                std::back_inserter(result));
    }
    while (count != 0);

    g_in->seekg(0, std::ios::beg);
    return result;
}
