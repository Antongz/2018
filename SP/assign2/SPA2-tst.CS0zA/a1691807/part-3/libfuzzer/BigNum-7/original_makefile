CC = gcc
CFLAGS = -Wall -std=c99
all: calc
calc: calc.o libbn.a
	$(CC) calc.o libbn.a -o calc
calc.o: calc.c
	$(CC) -c $(CFLAGS) calc.c
libbn.a: bn.c
	$(CC) -c $(CFLAGS) bn.c -o libbn.a
clean:
	rm -rf *o calc