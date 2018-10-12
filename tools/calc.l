%{

#include "calc.yacc.h"

void yyerror(char *s);

%}

%%
"print"         { return print; }
"exit"          { return exit_command; }
[a-zA-Z]        { yylval.id = yytext[0]; return identifier; }
[0-9]+          { yylval.num = atoi(yytext); return number; }
[ \t\n]         ;
[-+=;]          { return yytext[0]; }
.               { ECHO; yyerror("unexpected character"); }
%%

int yywrap() {
    return 1;
}
