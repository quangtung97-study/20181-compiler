%{
    #include "pl0.yacc.h"
    #include <stdlib.h>
%}

DIGIT       [0-9]
ALPHA       [A-Za-z]
ALPHADIGIT  [A-Za-z0-9]
WHITESPACE  [ \n\t\r]

%x COMMENT_SINGLE
%x COMMENT_MULTIPLE

%%

<INITIAL>"//"           BEGIN(COMMENT_SINGLE);
<INITIAL>"(*"           BEGIN(COMMENT_MULTIPLE);
<COMMENT_SINGLE,COMMENT_MULTIPLE>.  ;
<COMMENT_SINGLE>"\n"    BEGIN(INITIAL);
<COMMENT_MULTIPLE>"\n"  ;
<COMMENT_MULTIPLE>"*)"  BEGIN(INITIAL);

(?i:begin)              return TOKEN_BEGIN;
(?i:call)               return CALL;
(?i:const)              return CONST;
(?i:do)                 return DO;
(?i:else)               return ELSE;
(?i:end)                return END;

(?i:for)                return FOR;
(?i:if)                 return IF;
(?i:odd)                return ODD;
(?i:procedure)          return PROCEDURE;
(?i:program)            return PROGRAM;
(?i:then)               return THEN;
(?i:to)                 return TO;
(?i:var)                return VAR;
(?i:while)              return WHILE;

{ALPHA}{ALPHADIGIT}*    return IDENT; 
{DIGIT}+                return NUMBER;

"+"                     return '+';
"-"                     return '-';
"*"                     return '*';
"/"                     return '/';
"%"                     return '%';

"="                     return EQ;
"<>"                    return NE;
"<"                     return LT;
">"                     return GT;
"<="                    return LE;
">="                    return GE;

"("                     return '(';
")"                     return ')';
"["                     return '[';
"]"                     return ']';

"."                     return '.';
","                     return ',';
";"                     return ';';
":="                    return ASSIGN;
{WHITESPACE}            ;
. { 
    printf("Khong nhan dien duoc ki tu: %s\n", yytext);
    exit(-1); 
}
%%

int yywrap() {
    return 1;
}

