%{
    extern int yylex();
    extern void yyerror(const char *s);   
%}

%token KEYWORD
%token IDENT
%token CONSTANT
%token STRING_LITERAL

%token BREAK
%token CASE
%token CHAR
%token CONST
%token CONTINUE
%token DEFAULT
%token DO
%token DOUBLE
%token ELSE
%token ENUM
%token FLOAT
%token FOR
%token GOTO
%token IF
%token INT
%token LONG
%token RETURN
%token SHORT
%token SIZEOF
%token STRUCT
%token SWITCH
%token UNION
%token UNSIGNED
%token VOID
%token WHILE

%token ARROW
%token INCREASE
%token DECREASE
%token SHIFT_LEFT
%token SHIFT_RIGHT

%token GT
%token LT
%token GE
%token LE
%token EQ
%token NE

%token AND
%token OR

%token ADD_ASSIGN
%token SUB_ASSIGN
%token MUL_ASSIGN
%token DIV_ASSIGN
%token REM_ASSIGN
%token SL_ASSIGN
%token SR_ASSIGN
%token AND_ASSIGN
%token OR_ASSIGN
%token XOR_ASSIGN

%start translation_unit

%%
translation_unit
    : external_declaration
    | translation_unit external_declaration
    ;

external_declaration
    : init_declaration
    | function_declaration
    | function_definition
    ;

function_declaration 
    : declaration_specifier function_declarator 
        '(' parameter_list_opt ')' ';' ;

function_definition
    : declaration_specifier function_declarator 
        '(' parameter_list_opt ')' 
        compound_statement ;

parameter_list_opt
    : %empty
    | parameter_list
    ;

parameter_list
    : declaration_specifier declarator 
    | parameter_list ',' declaration_specifier declarator
    ;

function_declarator
    : IDENT
    | pointer_seq IDENT
    ;

const_opt
    : %empty
    | CONST
    ;

declaration_specifier: const_opt type_specifier ;

type_specifier
    : VOID
    | UNSIGNED type_integer
    | type_integer
    | FLOAT 
    | DOUBLE
    | struct_or_union_specifier
    | enum_specifier
    ;

type_integer
    : CHAR
    | SHORT
    | INT
    | LONG
    ;

init_declaration
    : declaration_specifier init_declarator_list ';'
    | declaration_specifier ';'
    ;

init_declarator_list 
    : init_declarator
    | init_declarator_list ',' init_declarator 
    ;

init_declarator 
    : declarator 
    | declarator '=' initializer
    ;

initializer
    : assignment_expression
    | '{' initializer_list '}'
    | '{' initializer_list ',' '}'
    ;

initializer_list
    : initializer
    | initializer_list ',' initializer
    ;

declarator
    : direct_declarator 
    | pointer_seq direct_declarator
    ;

pointer: '*' const_opt

pointer_seq
    : pointer
    | pointer_seq pointer
    ;

assignment_expression_opt
    : %empty
    | assignment_expression
    ;

direct_declarator
    : IDENT
    | direct_declarator '[' assignment_expression_opt ']'
    ;

ident_opt
    : %empty
    | IDENT
    ;

struct_or_union_specifier
    : struct_or_union ident_opt '{' struct_declaration_seq '}'
    | struct_or_union ident_opt '{' '}'
    | struct_or_union IDENT

struct_or_union
    : STRUCT
    | UNION
    ;

struct_declaration_seq
    : struct_declaration
    | struct_declaration_seq struct_declaration
    ;

struct_declaration
    : declaration_specifier struct_declarator_list ';' 
    ;

struct_declarator_list
    : declarator
    | struct_declarator_list ',' declarator
    ;

enum_specifier
    : ENUM ident_opt '{' enumerator_list '}'
    | ENUM ident_opt '{' enumerator_list ',' '}'
    | ENUM IDENT
    ;

enumerator_list
    : enumerator
    | enumerator_list ',' enumerator
    ;

enumerator
    : IDENT 
    | IDENT '=' constant_expression
    ;

primary_expression
    : IDENT
    | CONSTANT
    | STRING_LITERAL
    | '(' expression ')'
    ;

postfix_expression
    : primary_expression
    | postfix_expression '[' expression ']'
    | postfix_expression '(' argument_expression_list ')'
    | postfix_expression '(' ')'
    | postfix_expression '.' IDENT
    | postfix_expression ARROW IDENT
    | postfix_expression INCREASE
    | postfix_expression DECREASE
    ;

argument_expression_list
    : assignment_expression
    | argument_expression_list ',' assignment_expression
    ;

unary_expression
    : postfix_expression
    | INCREASE unary_expression
    | DECREASE unary_expression
    | unary_operator cast_expression
    | SIZEOF unary_expression
    /* | SIZEOF '(' type_name ')' */
    ;

unary_operator
    : '&'
    | '*'
    | '+'
    | '-'
    | '~'
    | '!'
    ;

cast_expression
    : unary_expression
    /* | '(' type_name ')' cast_expression */
    ;

multiplicative_expression
    : cast_expression
    | multiplicative_expression '*' cast_expression
    | multiplicative_expression '/' cast_expression
    | multiplicative_expression '%' cast_expression
    ;

additive_expression
    : multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;

shift_expression
    : additive_expression
    | shift_expression SHIFT_LEFT additive_expression
    | shift_expression SHIFT_RIGHT additive_expression
    ;

relational_expression
    : shift_expression
    | relational_expression LT shift_expression
    | relational_expression GT shift_expression
    | relational_expression LE shift_expression
    | relational_expression GE shift_expression
    ;

equality_expression
    : relational_expression
    | equality_expression EQ relational_expression
    | equality_expression NE relational_expression
    ;

and_expression
    : equality_expression
    | and_expression '&' equality_expression
    ;

xor_expression
    : and_expression
    | xor_expression '^' and_expression
    ;

or_expression
    : xor_expression
    | or_expression '|' xor_expression
    ;

logical_and_expression
    : or_expression
    | logical_and_expression AND or_expression
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR logical_and_expression
    ;

conditional_expression
    : logical_or_expression
    | logical_or_expression '?' expression ':' conditional_expression
    ;

assignment_expression
    : conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;

assignment_operator
    : '='
    | ADD_ASSIGN
    | SUB_ASSIGN
    | MUL_ASSIGN
    | DIV_ASSIGN
    | REM_ASSIGN
    | AND_ASSIGN
    | OR_ASSIGN
    | XOR_ASSIGN
    | SL_ASSIGN
    | SR_ASSIGN
    ;

expression
    : assignment_expression 
    | expression ',' assignment_expression
    ;

constant_expression: conditional_expression ;

statement
    : labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jmp_statement
    ;

labeled_statement
    : IDENT ':' statement
    | CASE constant_expression ':' statement
    | DEFAULT ':' statement
    ;

compound_statement: '{' block_item_seq_opt '}' ;

block_item_seq_opt
    : %empty 
    | block_item_seq
    ;

block_item_seq
    : block_item
    | block_item_seq block_item
    ;

block_item
    : init_declaration
    | statement
    ;

expression_statement
    : ';'
    | expression ';'
    ;

selection_statement
    : IF '(' expression ')' statement 
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;

expression_opt
    : %empty
    | expression
    ;

iteration_statement
    : WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_opt ';' expression_opt ';' expression_opt ')' statement
    | FOR '(' init_declaration expression_opt ';' expression_opt ')' statement
    ;

jmp_statement
    : GOTO IDENT ';'
    | CONTINUE ';'
    | BREAK ';'
    | RETURN expression_opt ';'
    ;

%%
