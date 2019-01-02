calcMake:	libbn.a
	gcc -o calc libbn.a -Wall -lm

libbn.a:    bn.o calc.o
	ar rc libbn.a calc.o bn.o
	ranlib libbn.a

calculator.o:	calc.c bn.h
	gcc -c calc.c -Wall

bn.o:	bn.c bn.h
	gcc -c bn.c -Wall

