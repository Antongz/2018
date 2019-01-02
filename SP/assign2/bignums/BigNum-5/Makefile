all: calc

bn.o: bn.h bn.c
	gcc -std=c99 -Wall -c bn.c -o bn.o

libbn.a: bn.o
	ar cr libbn.a bn.o

bn_stack.o: bn_stack.h bn_stack.c libbn.a
	gcc -std=c99 -Wall -c bn_stack.c -o bn_stack.o

calc.o: calc.c bn_stack.o
	gcc -std=c99 -Wall -c calc.c -o calc.o

calc: calc.o bn_stack.o libbn.a
	gcc -std=c99 -Wall calc.o bn_stack.o -o calc -L. -lbn -lm

clean:
	rm -f *.o *.a calc
