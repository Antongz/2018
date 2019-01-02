/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/16/2018 09:40:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:
 *
 * =====================================================================================
 */
#include "calc.c"

int main() {
  char* line;
  char** strings;
  bn_stack* stack = create_stack();
  while ((line = read_line(stdin)) != NULL) {
    strings = splitline(line);
    char** ptr = strings;

    // Process commands.
    while (*ptr != NULL) {
      char* command = *ptr;

      // If a valid number add it to the stack.
      if (is_number(command)) {

        bn_t new_number = bn_alloc();
        bn_fromString(new_number, command);
        stack_push(stack, new_number);

      } else if (strcmp(command, "+") == 0 || strcmp(command, "-") == 0 || strcmp(command, "*") == 0) {

        bn_t a = stack_pop(stack);
        bn_t b = stack_pop(stack);
        if (a == NULL || b == NULL) {
          error("Bad input: Need numbers first.");
        }
        bn_t result = bn_alloc();
        switch (command[0]) {
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

      } else if (strcmp(command, "dup") == 0) {

        bn_t a = stack_peek(stack);
        if (a == NULL) {
          error("Bad input: Nothing to duplicate.");
        }
        bn_t b = bn_alloc();
        bn_copy_deep(a, b);
        stack_push(stack, b);

      } else if (strcmp(command, "pop") == 0) {

        stack_pop(stack);

      } else if (strcmp(command, "print") == 0) {

        bn_t top_bn = stack_peek(stack);
        if (top_bn == NULL) {
          error("Bad input: Nothing to print.");
        }
        bn_print(top_bn);

      } else if (strcmp(command, "swap") == 0) {

        bn_t a = stack_pop(stack);
        bn_t b = stack_pop(stack);
        if (a == NULL || b == NULL) {
          error("Bad input: Not enough numbers on the stack.");
        }
        stack_push(stack, a);
        stack_push(stack, b);

      } else if (strcmp(command, "dump") == 0) {

        dump_stack(stack);

      } else if (strcmp(command, "clear") == 0) {

        clear_stack(stack);

      } else {
        error("Bad input: Command is not valid");
      }

      ptr++;
    }
  }

  return 0;
}

