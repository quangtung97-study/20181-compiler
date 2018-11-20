.PHONY: all
all: 
	bison -d -o calc.yacc.c calc.y
	flex -o calc.lex.c calc.l
	gcc main.c calc.yacc.c calc.lex.c -o main && ./main < input
