CC=afl-gcc -fno-stack-protector -z execstack
calc:
	$(CC) -c bn.c -o bn.o -Wall -std=c99
	ar rcs libbn.a bn.o
	$(CC) calc.c -L. -lbn -o calc -Wall
