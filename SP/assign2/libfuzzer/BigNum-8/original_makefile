calc:
	gcc -c bn.c -o bn.o -Wall -std=c99
	ar rcs libbn.a bn.o
	gcc calc.c -L. -lbn -o calc -Wall
