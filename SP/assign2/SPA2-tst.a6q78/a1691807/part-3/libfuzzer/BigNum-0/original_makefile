CFLAGS=-g -Wall -Wextra -I. $(OPTFLAGS)

LIBS=libbn.a $(OPTLIBS)

TARGET_LIB=libbn.a
TARGET=calc
OBJECTS=bn.o util.o

all: $(TARGET)

$(TARGET_LIB): $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(TARGET): $(TARGET_LIB) 
	$(CC) -o $@ calc.c $(CFLAGS) -L. -lbn 

clean:
	rm $(TARGET) $(TARGET_LIB) $(OBJECTS)

