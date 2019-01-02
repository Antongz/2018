FLAGS := -O0 -g -Wall -Wextra -Wpedantic
CC := clang-7
AFLCC := afl-clang

.PHONY: all clean

all: calc calc_asan calc_msan calc_ubsan calc_afl calc_afl_asan calc_fuzz
clean:
	rm calc*

calc: bn.c calc.c
	${CC} ${FLAGS} -o $@ calc.c

calc_asan: bn.c calc.c
	${CC} ${FLAGS} -o $@ -fsanitize=address calc.c

calc_msan: bn.c calc.c
	${CC} ${FLAGS} -o $@ -fsanitize=memory calc.c

calc_ubsan: bn.c calc.c
	${CC} ${FLAGS} -o $@ -fsanitize=undefined calc.c

calc_afl: bn.c calc.c
	${AFLCC} ${FLAGS} -o $@ calc.c

calc_afl_asan: bn.c calc.c
	${AFLCC} ${FLAGS} -o $@ -fsanitize=address calc.c

calc_fuzz: bn.c fuzz.c
	${CC} ${FLAGS} -o $@ -fsanitize=fuzzer,address calc.c
