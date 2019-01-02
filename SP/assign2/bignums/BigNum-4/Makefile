all: libbn.a calc

libbn.a: bn.c bn.h
	gcc --std=gnu99 -Wall -O2 -c -o libbn.a bn.c

calc: calc.c calc.h bn.c
	gcc --std=gnu99 -O2 -Wall -o calc calc.c bn.c

clean:
	rm libbn.a calc
