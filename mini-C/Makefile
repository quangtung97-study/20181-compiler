.PHONY: all
all:
	bison -d -Wconflicts-sr -o miniC.yacc.c miniC.y
	flex -o miniC.lex.c miniC.l
	gcc main.c miniC.yacc.c miniC.lex.c -o main
	main < input
