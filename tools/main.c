#include "main.h"
#include <stdio.h>
#include <stdlib.h>

static int symbols[52];

void exitProgram() {
    exit(0);
}

int isLower(char token) {
    if (token <= 'z' && token >= 'a')
        return 1;
    return 0;
}

int computeSymbolIndex(char token) {
    int index = -1;
    if (isLower(token))
        index = token - 'a' + 26;
    else
        index = token - 'A';
    return index;
}

int symbolVal(char symbol) {
    int bucket = computeSymbolIndex(symbol);
    return symbols[bucket];
}

void updateSymbolVal(char symbol, int value) {
    int bucket = computeSymbolIndex(symbol);
    symbols[bucket] = value;
}

void printNumber(int num) {
    printf("%d\n", num);
}

int main() {
    for (int i = 0; i < 52; i++)
        symbols[i] = 0;
    return yyparse();
}

void yyerror(char *s) { fprintf(stderr, "%s\n", s); }


