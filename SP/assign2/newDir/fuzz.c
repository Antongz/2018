#include "calc.c"

int LLVMFuzzerTestOneInput(const uint8_t *data, uint64_t size) {
    // Copy the data so we can mutate it
    char* d = malloc(size + 1);
    memcpy(d, data, size);
    d[size] = '\0';

    if (parse(d) == -1) {
        fprintf(stderr, "Error\n");
        return -1;
    }

    free(d);
    return 0;
}
