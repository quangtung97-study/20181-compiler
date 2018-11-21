#include "pl0.yacc.h"
#include <stdio.h>

void yyerror(const char *s) {
    printf("%s\n", s);
}

int main() {
    int res = yyparse();
    if (res == 0)
        printf("Da nhan dien chuong trinh\n");
    return res;
}
