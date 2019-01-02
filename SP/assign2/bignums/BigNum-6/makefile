clean: calc
	rm -f *.o *.gch

calc: calculator.o stack.o libbn.a
	gcc -Wall -std=gnu99 -lm -o calc calculator.o stack.o -L. -lbn
	
bn.o: bn.c bn.h
	gcc -Wall -std=gnu99 -O -c bn.c

stack.o: stack.c stack.h bn.o
	gcc -Wall -std=gnu99 -O -c stack.c
	
calculator.o: calculator.c
	gcc -Wall -std=gnu99 -O -c calculator.c

libbn.a: bn.o 
	ar rcs libbn.a bn.o 

