linux:	
	bison -dy shell.y 
	flex shell.lex
	gcc lex.yy.c y.tab.c y.tab.h shell.h shell.c main.c
	./a.out
