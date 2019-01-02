FLAGS := -O0 -g -Wall -Wextra -Wpedantic
CC := clang-7
AFLCC := afl-clang

.PHONY: all clean

all: calc calc_asan calc_msan calc_ubsan calc_afl calc_afl_asan calc_fuzz
clean:  
	rm calc_* calc

calc: bn.c main.c
	${CC} ${FLAGS} -o $@ main.c

calc_asan: bn.c main.c
	${CC} ${FLAGS} -o $@ -fsanitize=address main.c

calc_msan: bn.c main.c
	${CC} ${FLAGS} -o $@ -fsanitize=memory main.c

calc_ubsan: bn.c main.c
	${CC} ${FLAGS} -o $@ -fsanitize=undefined main.c

calc_afl: bn.c main.c
	${AFLCC} ${FLAGS} -o $@ main.c

calc_afl_asan: bn.c main.c
	${AFLCC} ${FLAGS} -o $@ -fsanitize=address main.c

calc_fuzz: bn.c fuzz.c
	${CC} ${FLAGS} -o $@ -fsanitize=fuzzer,address fuzz.c
