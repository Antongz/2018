CFLAGS=-g -std=gnu99 -I../../src
LDFLAGS=-L../../src/ -g
LDLIBS=-lmastik -ldwarf -lelf -lbfd

all: num

num: num.c ybn-1.c ybn.h
	${CC} ${LDFLAGS} -o num num.c ybn-1.c ${LDLIBS}

clean:
	rm -r *.o num


