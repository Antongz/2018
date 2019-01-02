#include "bn.h"

#ifndef __BN_STACK_H__
#define __BN_STACK_H__ 1

typedef struct bn_stack *bn_s;

bn_s init_stack();
void stack_free(bn_s stack);

int stack_push(bn_s stack, bn_t bn);
int stack_pop(bn_s stack);
bn_t stack_top(bn_s stack);
int stack_print(bn_s stack);
void stack_clear(bn_s stack);

#endif // __BN_H__
