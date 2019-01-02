all: calc

calc:	calc.o libbn.a
	gcc -std=c99 -L -lb -o calc calc.o libbn.a -Wall 

calc.o:	calc.c bn.h
	gcc -std=c99 -c calc.c -Wall

bn.o: bn.c bn.h
	gcc -std=c99 -c bn.c -Wall

libbn.a:    bn.o
	ar rcs libbn.a bn.o

libs:	libbn.a

clean:
	rm -f calc *.o *.a

