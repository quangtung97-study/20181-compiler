%{
#include <string.h>
#include <ctype.h>

enum Token {
    TOKEN_IDENT = 1,
    TOKEN_NUMBER,

    TOKEN_BEGIN, TOKEN_CALL, TOKEN_CONST, TOKEN_DO, 
    TOKEN_ELSE, TOKEN_END, 
    TOKEN_FOR, TOKEN_IF, TOKEN_ODD, 
    TOKEN_PROCEDURE, TOKEN_FUNCTION,
    TOKEN_PROGRAM, TOKEN_THEN, TOKEN_TO, 
    TOKEN_VAR, TOKEN_WHILE, 

    TOKEN_PLUS, TOKEN_MINUS, TOKEN_TIMES, TOKEN_DIVIDE, TOKEN_REMAINDER,

    TOKEN_EQ, TOKEN_NE, TOKEN_LT, TOKEN_GT, TOKEN_LE, TOKEN_GE,

    TOKEN_LPARENT, TOKEN_RPARENT, 
    TOKEN_LBRACKET, TOKEN_RBRACKET,

    TOKEN_PERIOD, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_ASSIGN,
};

#define IDENT_NAME_SIZE 10
#define MAX_DIGIT_COUNT 9

%}

DIGIT       [0-9]
ALPHA       [A-Za-z]
ALPHADIGIT  [A-Za-z0-9]
WHITESPACE  [ \n\t\r]

%%

(?i:begin)              return TOKEN_BEGIN;
(?i:call)               return TOKEN_CALL;
(?i:const)              return TOKEN_CONST;
(?i:do)                 return TOKEN_DO;
(?i:else)               return TOKEN_ELSE;
(?i:end)                return TOKEN_END;

(?i:for)                return TOKEN_FOR;
(?i:if)                 return TOKEN_IF;
(?i:odd)                return TOKEN_ODD;
(?i:procedure)          return TOKEN_PROCEDURE;
(?i:function)           return TOKEN_FUNCTION;
(?i:program)            return TOKEN_PROGRAM;
(?i:then)               return TOKEN_THEN;
(?i:to)                 return TOKEN_TO;
(?i:var)                return TOKEN_VAR;
(?i:while)              return TOKEN_WHILE;

"+"                     return TOKEN_PLUS;
"-"                     return TOKEN_MINUS;
"*"                     return TOKEN_TIMES;
"/"                     return TOKEN_DIVIDE;
"%"                     return TOKEN_REMAINDER;

"="                     return TOKEN_EQ;
"<>"                    return TOKEN_NE;
"<"                     return TOKEN_LT;
">"                     return TOKEN_GT;
"<="                    return TOKEN_LE;
">="                    return TOKEN_GE;

"("                     return TOKEN_LPARENT;
")"                     return TOKEN_RPARENT;
"["                     return TOKEN_LBRACKET;
"]"                     return TOKEN_RBRACKET;

"."                     return TOKEN_PERIOD;
","                     return TOKEN_COMMA;
";"                     return TOKEN_SEMICOLON;
":="                    return TOKEN_ASSIGN;


{ALPHA}{ALPHADIGIT}*    return TOKEN_IDENT; 
{DIGIT}+                return TOKEN_NUMBER;
{WHITESPACE}            ;
. { 
    printf("Khong nhan dien duoc ki tu: %s\n", yytext);
    exit(-1); 
}

%%

int yywrap() {
    return 1;
}

int main() {
    FILE *file = fopen("input", "rb");
    if (file == NULL) {
        printf("File not found\n");
        return -1;
    }
    yyrestart(file);

    int token;
    while (token = yylex()) {
        if (token == TOKEN_IDENT) {
            char str[256];
            strncpy(str, yytext, IDENT_NAME_SIZE);
            str[IDENT_NAME_SIZE] = '\0';
            printf("Token: %s\n", str);
        }
        else if (token == TOKEN_NUMBER) {
            int i = 0;
            for (; yytext[i]; i++)
                if (yytext[i] != '0')
                    break;
            if (strlen(yytext) - i > MAX_DIGIT_COUNT) {
                printf("So vuot qua 9 chu so!!\n");
                exit(-1);
            }
            int num = atoi(yytext + i);
            printf("Number: %d\n", num);
        }
        else {
            for (int i = 0; yytext[i]; i++) 
                yytext[i] = tolower(yytext[i]);
            printf("%s\n", yytext);
        }
    }

    fclose(file);
    return 0;
}
