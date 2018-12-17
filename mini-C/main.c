#include "miniC.yacc.h"
#include <stdio.h>

void yyerror(const char *s) {
    printf("ERROR: %s\n", s);
}

int main() {
    return yyparse();
}
