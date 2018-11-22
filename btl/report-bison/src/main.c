#include "calc.yacc.h"
#include <stdio.h>

void yyerror(const char *s) {
    printf("%s\n", s);
}

static int g_number;
void set_number(int n) {
    g_number = n;
}

int main() {
    int res = yyparse();
    if (res == 0)
        printf("%d\n", g_number);
    return res;
}
