#ifndef MAIN_H
#define MAIN_H

void exitProgram();

int symbolVal(char symbol);

void updateSymbolVal(char symbol, int value);

void printNumber(int num);

void yyerror(char *s);

extern int yylex();

int yyparse();

#endif
