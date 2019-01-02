#include "calc.c"

int LLVMFuzzerTestOneInput(const uint8_t *data, uint64_t size) {
    // Copy the data so we can mutate it
    char* d = malloc(size + 1);
    memcpy(d, data, size);
    d[size] = '\0';
    bn_stack* stack = create_stack();
    // If a valid number add it to the stack.
    if (is_number(d)) {

        bn_t new_number = bn_alloc();
        bn_fromString(new_number, d);
        stack_push(stack, new_number);

    } else if (strcmp(d, "+") == 0 || strcmp(d, "-") == 0 || strcmp(d, "*") == 0) {

        bn_t a = stack_pop(stack);
        bn_t b = stack_pop(stack);
        if (a == NULL || b == NULL) {
            error("Bad input: Need numbers first.");
        }
        bn_t result = bn_alloc();
        switch (d[0]) {
        case '+':
            bn_add(result, a, b);
            break;
        case '-':
            bn_sub(result, b, a);
            break;
        case '*':
            bn_mul(result, a, b);
            break;
        }
        stack_push(stack, result);

    } else if (strcmp(d, "dup") == 0) {

        bn_t a = stack_peek(stack);
        if (a == NULL) {
            error("Bad input: Nothing to duplicate.");
        }
        bn_t b = bn_alloc();
        bn_copy_deep(a, b);
        stack_push(stack, b);

    } else if (strcmp(d, "pop") == 0) {

        stack_pop(stack);

    } else if (strcmp(d, "print") == 0) {

        bn_t top_bn = stack_peek(stack);
        if (top_bn == NULL) {
            error("Bad input: Nothing to print.");
        }
        bn_print(top_bn);

    } else if (strcmp(d, "swap") == 0) {

        bn_t a = stack_pop(stack);
        bn_t b = stack_pop(stack);
        if (a == NULL || b == NULL) {
            error("Bad input: Not enough numbers on the stack.");
        }
        stack_push(stack, a);
        stack_push(stack, b);

    } else if (strcmp(d, "dump") == 0) {

        dump_stack(stack);

    } else if (strcmp(d, "clear") == 0) {

        clear_stack(stack);

    } else {
        error("Bad input: d is not valid");
    }



    free(d);
    return 0;
}
