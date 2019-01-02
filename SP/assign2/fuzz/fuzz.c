#include "src.c"

int LLVMFuzzerTestOneInput(const uint8_t *data, uint64_t size) {
    // Copy the data so we can mutate it
    char* d = malloc(size + 1);
    memcpy(d, data, size);
    d[size] = '\0';

    calc* c = calc_alloc();
    parse(c, d);

    free(d);
    return 0;
}
