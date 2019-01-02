calc: main.c libbn.a
	gcc -o calc main.c libbn.a -lm -Wall

libbn.a:	bn.o
	ar rc libbn.a bn.o
	ranlib libbn.a

bn.o: bn.c bn.h
	gcc -c bn.c -o bn.o -lm -Wall
