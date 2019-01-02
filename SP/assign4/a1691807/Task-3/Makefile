CFLAGS=-g -std=gnu99
LDFLAGS=-L../src/ -g
LDLIBS=-lmastik -dwarf -lelf -lbfd

all: num

num: num.c ybn.c ybn.h
	${CC} ${CFLAGS} -o num num.c ybn.c ${LDLIBS}

clean:
	rm -f num
