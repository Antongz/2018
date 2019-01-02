FLAGS := -O0 -g -Wall -Wextra -Wpedantic
CC := clang-7
AFLCC := afl-clang

.PHONY: all clean

all: calc calc_asan calc_msan calc_ubsan calc_afl calc_afl_asan calc_fuzz
clean:
	rm calc_* calc

calc: libbn.a ybn/ybn.c calc.c main.c
	${CC} ${FLAGS} -o $@ main.c libbn.a ybn/ybn.c

calc_asan: libbn.a ybn/ybn.c calc.c main.c
	${CC} ${FLAGS} -o $@ -fsanitize=address main.c libbn.a ybn/ybn.c

calc_msan: libbn.a ybn/ybn.c calc.c main.c
	${CC} ${FLAGS} -o $@ -fsanitize=memory main.c libbn.a ybn/ybn.c

calc_ubsan: libbn.a ybn/ybn.c calc.c main.c
	${CC} ${FLAGS} -o $@ -fsanitize=undefined main.c libbn.a ybn/ybn.c

calc_afl: libbn.a ybn/ybn.c calc.c main.c
	${AFLCC} ${FLAGS} -o $@ main.c libbn.a ybn/ybn.c

calc_afl_asan: libbn.a ybn/ybn.c calc.c main.c
	${AFLCC} ${FLAGS} -o $@ -fsanitize=address main.c libbn.a ybn/ybn.c

calc_fuzz: libbn.a ybn/ybn.c calc.c fuzz.c
	${CC} ${FLAGS} -o $@ -fsanitize=fuzzer,address fuzz.c libbn.a ybn/ybn.c
