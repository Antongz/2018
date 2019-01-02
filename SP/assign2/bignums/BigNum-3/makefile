all: .o libbn calc

.o: bn.c calc.c
	gcc -Wall -c -o bn.o bn.c
	gcc -Wall -c -o calc.o calc.c

libbn: bn.o
	ar rcs libbn.a bn.o

calc: calc.o libbn.a
	gcc -Wall -o calc calc.o libbn.a

clean:
	rm -f calc *.o *.a
