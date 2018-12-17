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

%start translation_unit

%%
translation_unit
    : external_declaration
    | translation_unit external_declaration
    ;

external_declaration
    : function_definition
    | declaration
    ;

function_definition
    : IDENT ';'
    ;

declaration_list
    : declaration
    | declaration_list declaration
    ;

declaration
    : declaration_specifiers ';'
    | declaration_specifiers init_declaration_list ';'
    ;

declaration_specifiers: type_qualifier_opt type_specifier;

init_declaration_list
    : init_declarator
    | init_declaration_list ',' init_declarator
    ;

init_declarator
    : declarator
    | declarator '=' initializer
    ;

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

type_qualifier_opt
    : type_qualifier
    | %empty
    ;

type_qualifier: CONST ;

ident_opt
    : IDENT
    | %empty
    ;

struct_or_union_specifier
    : struct_or_union ident_opt '{' struct_declaration_list '}'
    | struct_or_union IDENT

struct_or_union
    : STRUCT
    | UNION
    ;

struct_declaration_list
    : struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration
    : declaration_specifiers struct_declarator_list

struct_declarator_list
    : struct_declarator
    | struct_declarator_list ',' struct_declarator
    ;

struct_declarator: declarator ;

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
    : enumerator_constant 
    | enumerator_constant '=' constant_expression
    ;

enumerator_constant: IDENT ;

declarator: pointer_opt direct_declarator ;

direct_declarator
    : IDENT
    | '(' declarator ')'
    | direct_declarator '[' ']'
    | direct_declarator '(' parameter_list ')'
    ;

pointer
    : '*' 
    | '*' type_qualifier
    ;

pointer_opt
    : %empty
    | pointer
    ;

parameter_list
    : parameter_declaration
    | parameter_list ',' parameter_declaration
    ;

parameter_list_opt
    : %empty
    | parameter_list
    ;

parameter_declaration: declaration_specifiers declarator ;

initializer
    : assignment_expression
    | '{' initializer_list '}'
    | '{' initializer_list ',' '}'
    ;

initializer_list
    : initializer
    | initializer_list ',' initializer
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
    | SIZEOF '(' type_name ')'
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
    | '(' type_name ')' cast_expression
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
    ;

expression
    : assignment_expression 
    | expression ',' assignment_expression
    ;

constant_expression: conditional_expression ;

type_name: declaration_specifiers abstract_declarator ;

abstract_declarator
    : pointer
    | pointer_opt direct_abstract_declarator
    ;

direct_abstract_declarator
    : '(' abstract_declarator ')'
    | direct_abstract_declarator_opt '[' ']'
    | direct_abstract_declarator_opt '(' parameter_list_opt ')'
    ;

direct_abstract_declarator_opt
    : %empty
    | direct_abstract_declarator
    ;

%%
