#include <stdio.h>
#include <string.h>

#include "bn.c"
#include "util.c"

// Simple stack for the calculator.
typedef struct bn_stack {
  bn_t* nums;
  int capacity;
  int size;
} bn_stack;


/**
 * Check if a given string is a valid number.
 *
 * @param number
 * @return 1 if it is number, 0 otherwise.
 */
int is_number(char* str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') return 0;
  }
  return 1;
}

/**
 * Allocates memory for a new stack.
 *
 * @return a pointer to the new stack.
 */
bn_stack* create_stack() {
  bn_stack* stack = emalloc(sizeof(bn_stack));
  stack->capacity = 10;
  stack->nums = emalloc(sizeof(bn_t) * stack->capacity);
  stack->size = 0;
  return stack;
}


/**
 * Pushes a given big number onto the stack.
 *
 * @param stack
 * @param big number to be pushed.
 */
void stack_push(bn_stack* stack, bn_t bn) {
  if (stack->size == stack->capacity) {
    stack->capacity *= 2;
    erealloc(stack->nums, sizeof(bn_t) * stack->capacity);
  }
  stack->nums[stack->size++] = bn;
}


/**
 * Prints a given big number.
 *
 * @param bn
 */
void bn_print(bn_t bn) {
  char* str = NULL;
  int len = 0;
  len = bn_toString(bn, str, len);
  str = emalloc(sizeof(char) * len);
  bn_toString(bn, str, len);
  printf("%s\n", str);
}


/**
 * Prints all the numbers on the stack.
 *
 * @param stack
 */
void dump_stack(bn_stack* stack) {
  int i;
  for (i = stack->size-1; i >= 0; i--) {
    bn_print(stack->nums[i]);
  }
}


/**
 * Clears the stack. Deleting all the numbers in it.
 *
 * @param stack
 */
void clear_stack(bn_stack* stack) {
  int i;
  for (i = stack->size-1; i >= 0; i--) {
    bn_free(stack->nums[i]);
  }
  stack->size = 0;
}


/**
 * Pops the top big num from the stack.
 *
 * @param stack.
 */
bn_t stack_pop(bn_stack* stack) {
  if (stack->size == 0) return NULL;
  return stack->nums[(stack->size--)-1];
}


/**
 * Returns the big num on the top of the stack.
 *
 * @param stack.
 */
bn_t stack_peek(bn_stack* stack) {
  if (stack->size == 0) return NULL;
  return stack->nums[stack->size-1];
}


//int main() {
//  char* line;
//  char** strings;
//  bn_stack* stack = create_stack();
//  while ((line = read_line(stdin)) != NULL) {
//    strings = splitline(line);
//    char** ptr = strings;

//    // Process commands.
//    while (*ptr != NULL) {
//      char* command = *ptr;

//      // If a valid number add it to the stack.
//      if (is_number(command)) {

//        bn_t new_number = bn_alloc();
//        bn_fromString(new_number, command);
//        stack_push(stack, new_number);

//      } else if (strcmp(command, "+") == 0 || strcmp(command, "-") == 0 || strcmp(command, "*") == 0) {

//        bn_t a = stack_pop(stack);
//        bn_t b = stack_pop(stack);
//        if (a == NULL || b == NULL) {
//          error("Bad input: Need numbers first.");
//        }
//        bn_t result = bn_alloc();
//        switch (command[0]) {
//          case '+':
//            bn_add(result, a, b);
//            break;
//          case '-':
//            bn_sub(result, b, a);
//            break;
//          case '*':
//            bn_mul(result, a, b);
//            break;
//        }
//        stack_push(stack, result);

//      } else if (strcmp(command, "dup") == 0) {

//        bn_t a = stack_peek(stack);
//        if (a == NULL) {
//          error("Bad input: Nothing to duplicate.");
//        }
//        bn_t b = bn_alloc();
//        bn_copy_deep(a, b);
//        stack_push(stack, b);

//      } else if (strcmp(command, "pop") == 0) {

//        stack_pop(stack);

//      } else if (strcmp(command, "print") == 0) {

//        bn_t top_bn = stack_peek(stack);
//        if (top_bn == NULL) {
//          error("Bad input: Nothing to print.");
//        }
//        bn_print(top_bn);

//      } else if (strcmp(command, "swap") == 0) {

//        bn_t a = stack_pop(stack);
//        bn_t b = stack_pop(stack);
//        if (a == NULL || b == NULL) {
//          error("Bad input: Not enough numbers on the stack.");
//        }
//        stack_push(stack, a);
//        stack_push(stack, b);

//      } else if (strcmp(command, "dump") == 0) {

//        dump_stack(stack);

//      } else if (strcmp(command, "clear") == 0) {

//        clear_stack(stack);

//      } else {
//        error("Bad input: Command is not valid");
//      }

//      ptr++;
//    }
//  }
  
//  return 0;
//}
