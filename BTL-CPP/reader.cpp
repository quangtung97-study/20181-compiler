#include "reader.h"
#include <stdlib.h>

#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0

static FILE *g_file;
static char g_buff[BUFF_SIZE];
static int g_buff_index;
static int g_buff_end;
static int g_going_newline;
static int g_col;
static int g_line;

static void init(FILE *fp) {
    g_file = fp;
    g_buff_index = 0;
    g_buff_end = 0;
    g_going_newline = TRUE;
    g_col = 1;
    g_line = 0;
}

void rd_set(FILE *fp) {
    init(fp);
    rd_next();
}

int rd_get() {
    return g_buff[g_buff_index];
}

void rd_next() {
    if (g_buff_end == g_buff_index) {
        int n = fread(g_buff, 1, BUFF_SIZE, g_file);
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

char *rd_all() {
    fseek(g_file, 0, SEEK_END);
    int size = ftell(g_file);
    fseek(g_file, 0, SEEK_SET);
    char *s = (char *)malloc(size + 1);
    fread(s, 1, size, g_file);
    s[size] = '\0';
    return s;
}
