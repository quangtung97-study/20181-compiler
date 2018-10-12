main: main.c  calc.yacc.c calc.lex.c
	gcc -o main $^

calc.yacc.c calc.yacc.h: calc.y
	yacc -d -o calc.yacc.c $^

calc.lex.c: calc.l calc.yacc.h
	lex -o calc.lex.c $<
