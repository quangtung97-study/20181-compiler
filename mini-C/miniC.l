%{
    #include "miniC.yacc.h"
    #include <stdlib.h>
%}

DIGIT       [0-9]
ALPHA       [_A-Za-z]
ALPHADIGIT  [_A-Za-z0-9]
WHITESPACE  [ \n\t\r]

%x COMMENT_SINGLE
%x COMMENT_MULTIPLE

%%

<INITIAL>"//"           BEGIN(COMMENT_SINGLE);
<INITIAL>"/*"           BEGIN(COMMENT_MULTIPLE);
<COMMENT_SINGLE,COMMENT_MULTIPLE>.  ;
<COMMENT_SINGLE>"\n"    BEGIN(INITIAL);
<COMMENT_MULTIPLE>"\n"  ;
<COMMENT_MULTIPLE>"*/"  BEGIN(INITIAL);

"break" return BREAK;
"case" return CASE;
"char" return CHAR;
"const" return CONST;
"continue" return CONTINUE;
"default" return DEFAULT;
"do" return DO;
"double" return DOUBLE;
"else" return ELSE;
"enum" return ENUM;
"float" return FLOAT;
"for" return FOR;
"goto" return GOTO;
"if" return IF;
"int" return INT;
"long" return LONG;
"return" return RETURN;
"short" return SHORT;
"sizeof" return SIZEOF;
"struct" return STRUCT;
"switch" return SWITCH;
"union" return UNION;
"void" return VOID;
"unsigned" return UNSIGNED;
"while" return WHILE;

{ALPHA}{ALPHADIGIT}* return IDENT;
{DIGIT}{DIGIT}* return CONSTANT;

"[" return '[';
"]" return ']';
"{" return '{';
"}" return '}';
"(" return '(';
")" return ')';
"." return '.';
"->" return ARROW;
"++" return INCREASE;
"--" return DECREASE;

"*" return '*';
"+" return '+';
"-" return '-';
"~" return '~';
"!" return '!';
"/" return '/';
"%" return '%';

"<<" return SHIFT_LEFT;
">>" return SHIFT_RIGHT;

">" return GT;
"<" return LT;
">=" return GE;
"<=" return LE;
"==" return EQ;
"!=" return NE;

"^" return '^';
"|" return '|';
"&" return '&';

"&&" return AND;
"||" return OR;

"?" return '?';
":" return ':';
";" return ';';
"," return ',';

"=" return '=';

{WHITESPACE}            ;
. { 
    printf("Khong nhan dien duoc ki tu: %s\n", yytext);
    exit(-1); 
}
%%

int yywrap() {
    return 1;
}

