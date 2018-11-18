#include "parser.h"

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

    ps_init(fp);
    ps_parse();

    printf("Nhan dien thanh cong!!!\n");

    fclose(fp);
    return 0;
}