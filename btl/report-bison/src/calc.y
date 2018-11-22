%{
    #include <stdio.h>
    #include <stdlib.h>

    extern int yylex();
    extern void yyerror(const char *s);
    extern void set_number(int n);
%}

%union {
    int num;
}
%token <num> NUMBER
%type <num> F U T E
%start E

%%
F   :   NUMBER      { $$ = $1; }
    |   '(' E ')'   { $$ = $2; }         
    |   '(' error ')' { printf("Thieu toan hang trong ngoac\n"); YYABORT; }
    |   '(' E error { printf("Thieu dau dong ngoac )\n"); YYABORT; }
    |   error       { printf("Thieu toan hang\n"); YYABORT; }
    ;
U   :   F           { $$ = $1; }
    |   '-' F       { $$ = -$2; }
    |   '+' F       { $$ = $2; }
    ;
T   :   U           { $$ = $1; }
    |   T '*' U     { $$ = $1 * $3; }
    |   T '/' U     { $$ = $1 / $3; }
    |   T '%' U     { $$ = $1 % $3; }
    ;
E   :   T           { $$ = $1; set_number($$); }
    |   E '+' T     { $$ = $1 + $3; set_number($$); }
    |   E '-' T     { $$ = $1 - $3; set_number($$); }
    |   E error T   { printf("Thieu phep toan\n"); YYABORT; }
    ;
%%
