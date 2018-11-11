#include "reader.h"
#include <ctype.h>
#include <stdlib.h>

void show(char ch) {
    if (ch == '\n')
        printf("LF");
    else if (ch == ' ')
        printf("SPC");
    else if (ch == '\r')
        printf("CR");
    else if (ch == '\0')
        printf("NULL");
    else {
        printf("%c", ch);
    }
}

int main() {
    FILE *fp = fopen("input", "rb");
    if (fp == NULL) {
        printf("Error while reading that file\n");
        return -1;
    }

    rd_set(fp);
    while (rd_get() != '\0') {
        show(rd_get());
        printf("\tpos: (%d, %d)\n", rd_line(), rd_col());
        rd_next();
    }

    char *s = rd_all();
    printf("%s", s);
    free(s);

    fclose(fp);
    return 0;
}
