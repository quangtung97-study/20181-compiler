.PHONY: all
all:
	bison -d -o pl0.yacc.c pl0.y
	flex -o pl0.lex.c pl0.l
	gcc main.c pl0.yacc.c pl0.lex.c -o main
	./main < input
