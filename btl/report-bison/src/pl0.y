%{
    extern int yylex();
    extern void yyerror(const char *s);   
%}

%token PROGRAM
%token IDENT
%token CONST
%token VAR
%token NUMBER
%token PROCEDURE
%token TOKEN_BEGIN
%token END
%token ASSIGN
%token CALL
%token IF
%token THEN
%token ELSE
%token WHILE
%token DO
%token FOR 
%token TO
%token ODD

%token EQ
%token GT
%token GE
%token LT
%token LE
%token NE

%start program
%nonassoc THEN
%nonassoc ELSE

%%
program     :   PROGRAM IDENT ';' block '.' ;

block       :   const_decl_opt var_decl_opt procedure_decl_seq begin_block ;

const_decl_opt  :   %empty
                |   CONST const_list ';'
                ;
const_list  :   const_list ',' IDENT EQ NUMBER 
            |   IDENT EQ NUMBER
            ;
var_decl_opt    :   %empty 
                |   VAR var_list ';'
                ;
var_list    :   var_list ',' IDENT array_decl_opt 
            |   IDENT array_decl_opt
            ;
array_decl_opt  :   %empty
                |   '[' NUMBER ']'
                ;
procedure_decl_seq  :   procedure_decl_seq procedure_decl
                    |   %empty 
                    ;
procedure_decl  :   PROCEDURE IDENT procedure_params_opt ';' block ';' ;

procedure_params_opt    :   %empty 
                        |   '(' param_list ')' 
                        ;
param_list     :   param_list ';' var_opt IDENT 
                |   var_opt IDENT
                ;
var_opt     :   %empty
            |   VAR
            ;

begin_block     :   TOKEN_BEGIN stmt_list END ;

stmt_list   : stmt_list ';' stmt
            | stmt
            ;
stmt    :   assign_stmt
        |   call_stmt
        |   begin_block
        |   if_stmt
        |   while_stmt
        |   for_stmt
        |   %empty
        ;
assign_stmt     :   IDENT array_subscript_opt ASSIGN expr ;

array_subscript_opt     :   %empty 
                        |   '[' expr ']'
                        ;
call_stmt   :   CALL IDENT call_subscript_opt ;

call_subscript_opt  :   %empty 
                    |   '(' expr_list ')'
                    ;

expr_list   :   expr_list ',' expr 
            |   expr 
            ;
if_stmt     :   IF cond THEN stmt 
            |   IF cond THEN stmt ELSE stmt
            ;
while_stmt  :   WHILE cond DO stmt ;

for_stmt    :   FOR IDENT ASSIGN expr TO expr DO stmt ;

expr    :   expr '+' term 
        |   expr '-' term 
        |   term 
        |   '-' term 
        |   '+' term
        ;
term    :   term '*' factor
        |   term '/' factor
        |   term '%' factor
        |   factor 
        ;
factor  :   IDENT array_subscript_opt 
        |   NUMBER
        |   '(' expr ')'
        ;
cond    :   ODD expr 
        |   expr comparison_operator expr
        ;
comparison_operator     :   EQ
                        |   GT
                        |   GE
                        |   LT
                        |   LE
                        |   NE
                        ;
%%
