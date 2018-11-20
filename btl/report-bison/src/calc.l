%{
    #include "calc.yacc.h"
    #include <stdio.h>
%}

%%
[0-9]+      { yylval.num = atoi(yytext); return NUMBER; }
"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }
"("         { return '('; }
")"         { return ')'; }
[ \t\b\r\n]      ;
.   { 
    printf("Khong nhan dien duoc ki tu: "); 
    ECHO; 
    printf("\n"); 
}
%%

int yywrap() {
    return 1;
}
