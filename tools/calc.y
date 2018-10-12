%{
#include "main.h"
%}

%union { int num; char id; }
%start line
%token print
%token exit_command
%token <num> number
%token <id> identifier
%type <num> line exp term
%type <id> assignment

%%

line    :   assignment ';'              {;}
        |   exit_command ';'            { exitProgram(); }
        |   print exp ';'               { printNumber($2); }
        |   line assignment ';'         {;}
        |   line print exp ';'          { printNumber($3); }
        |   line exit_command ';'       { exitProgram();}
        ;

assignment  :   identifier '=' exp      { updateSymbolVal($1, $3); }

exp     :   term                        { $$ = $1; }
        |   exp '+' term                { $$ = $1 + $3; }
        |   exp '-' term                { $$ = $1 - $3; }
        ;

term    :   number                      { $$ = $1; }
        |   identifier                  { $$ = symbolVal($1); }
        ;

%%      /* C code */
