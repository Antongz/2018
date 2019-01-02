#include "calc.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, uint64_t size) {
    // Copy the data so we can mutate it
    char* d = malloc(size + 1);
    memcpy(d, data, size);
    d[size] = '\0';

    if (parse(d) == -1) {
        fprintf(stderr, "Error\n");
        free(d);
        return -1;
    }

    free(d);
    return 0;
}
