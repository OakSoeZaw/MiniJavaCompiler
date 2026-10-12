all:
	bison -t -d -v parser.y
	flex lexer.l
	gcc lex.yy.c -o lexer  -lfl