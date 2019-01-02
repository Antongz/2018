LIBSRCS=ybn.c
LIBOBJS=${LIBSRCS:.c=.o}
LIB=libybn.a
CFLAGS= -g -Wall 
LDFLAGS=-g
LDLIBS=${LIB} 

all: ${LIB}

${LIB}: ${LIBOBJS}
	${AR} rcs ${LIB} ${LIBOBJS}

ybn.o: ybn.h

clean:
	rm -f ${LIB} ${LIBOBJS} 
