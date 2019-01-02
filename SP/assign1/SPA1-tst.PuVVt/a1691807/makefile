###The code below is written by Yi-Ting, Hsieh(a1691807)###

all: calc

calc:	calc.o libbn.a
	gcc -o calc calc.o libbn.a -Wall -lm

calc.o:	calc.c bn.h
	gcc -c calc.c -Wall

bn.o: bn.c bn.h
	gcc -c bn.c -Wall

libbn.a:    bn.o
	ar rcs libbn.a bn.o

libs:	libbn.a

clean:
	rm -f calc *.o *.a

