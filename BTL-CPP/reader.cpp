#include "reader.hpp"

#define BUFF_SIZE 1024

static FILE *g_file;

static char __g_buff[BUFF_SIZE + 4];
static char *g_first, *g_last;

static bool g_going_newline;
static int g_col;
static int g_line;

static char *buff_begin() {
    return __g_buff + 4;
}

void rd_set(FILE *file) {
    g_file = file;
    g_going_newline = true;
    g_line = 0;

    g_first = buff_begin();
    g_last = g_first + 1;

    rd_next();
}

static int __get() {
    return *g_first;
}

int rd_get() {
    return __get();
}

static void __next() {
    g_first++;
    if (g_first == g_last) {
        size_t amount = std::fread(
                buff_begin(), 1, BUFF_SIZE, g_file);
        if (amount == BUFF_SIZE) {
            g_first = buff_begin();
            g_last = g_first + amount;
        }
        else {
            g_first = buff_begin();
            g_last = g_first + amount + 1;
            *(g_last - 1) = '\0';
        }
    }
}

void rd_next() {
    __next();
    
    if (g_going_newline) {
        g_going_newline = false;
        g_line++;
        g_col = 1;
    }
    else {
        g_col++;
    }

    if (__get() == '\r')
        __next();

    if (__get() == '\n')
        g_going_newline = true;
}

int rd_col() { return g_col; }

int rd_line() { return g_line; }

void rd_reset() {
    std::fseek(g_file, 0, SEEK_SET);
    g_going_newline = true;
    g_line = 0;

    g_first = buff_begin();
    g_last = g_first + 1;
    rd_next();
}

bool rd_line(std::string& result) {
    if (rd_get() == '\0')
        return false;

    result.clear();
    while (rd_get() != '\n' && rd_get() != '\0') {
        result.push_back(rd_get());
        rd_next();
    }
    if (rd_get() == '\n')
        rd_next();
    return true;
}
