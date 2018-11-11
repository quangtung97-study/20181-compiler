#include "scanner.h"
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Thieu tham so\n");
        return -1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("Khong the doc file\n");
        return -1;
    }

    rd_set(fp);
    sc_init();

    while (sc_get() != END_OF_TOKENS) {
        sc_print(sc_get());
        sc_next();
    }

    fclose(fp);
    return 0;
}
