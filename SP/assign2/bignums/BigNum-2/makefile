
all: lib main

lib: bn.c
	gcc -c bn.c -o bignum.o -Wall
	ar -rcs libbn.a bignum.o

main: main.c
	gcc main.c -L . -lbn -o calc -Wall
