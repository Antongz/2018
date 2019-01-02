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
int main(int argc, const char* argv[]) {
    struct Stack* stack = new_stack(500);

    char input[5000];
    while(!feof(stdin)) {
        input[0] = '\0';
        fgets(input, 5000, stdin);
        if(parse_input(stack, input)) {
            return 1;
        }
    }

    return 0;
}

