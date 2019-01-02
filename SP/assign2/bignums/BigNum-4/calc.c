#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "bn.h"
#include "calc.h"

struct stack_t
{
    int top;      // Current position of top of stack
    int size;     // Maximum number of items the stack can hold.
    bn_t * array; // Holds bignums data 
};

/*
*  Allocates a new stack
*/
bn_stack bn_stackAlloc(){
    bn_stack stack = (bn_stack) malloc(sizeof( struct stack_t));
    if (stack == NULL){
      return NULL;
        fprintf(stderr, "Error allocating stack\n");
        exit(0);
    }
    stack->array = (bn_t * )calloc(10, sizeof(bn_t));
    if (stack->array == NULL){
        free(stack);
        return NULL;
        fprintf(stderr, "Error allocating stack\n");
        exit(0);
    }
    stack->top = 0;
    stack->size = 10;
    return stack;
}
/*
* Creates a bignum from str and puts it on the top of the stack
*/
int push(bn_t bn, bn_stack stack){
    // Increase stack size if stack full
     if (stack->top + 1 == stack->size){
        int newSize = stack->size * 2;
        stack->array = realloc(stack->array, newSize * sizeof(bn_t));
        
        if (stack->array == NULL){
            bn_free(bn);
            fprintf(stderr, "Error could not push to null stack\n");
            return -1;
        }
        for (int i=stack->size;i<newSize;i++){
            stack->array[i] = 0;
        }
        stack->size = newSize;
    }
    stack->top += 1;
    stack->array[stack->top] = bn;
    return 0;
}

/*
* Removes top value of stack
*/
int pop(bn_stack stack){
    if (stack->top > 0){
        bn_t result = stack->array[stack->top];

        if (result == NULL){
            fprintf(stderr, "Error NULL pop result\n");
            exit(0);
        }

        stack->top -= 1;

        // Shrink stack if top below half current capacity (minimum 10)
        if ((stack->top == (stack->size+1)/2) && (stack->top > 10)){
            stack->array = realloc(stack->array, (stack->size+1)/2 * sizeof(bn_t));
            if (stack->array == NULL){
                fprintf(stderr, "Error unable to shrink stack\n");
                return -1;
            }
            stack->size = (stack->size+1)/2;
        }
    }
    else{
        fprintf(stderr, "Error cannot pop an empty stack\n");
        return -1;
    }
    return 0;
}

/*
*   Prints the value of the bignum at the top of the stack
*/
int print(bn_stack stack){
    if (stack->top > 0){
        int bufSize = bn_toString(stack->array[stack->top], "", 0);
        char * buf = (char*) malloc(bufSize * sizeof(char));
        bn_toString(stack->array[stack->top], buf, bufSize);
        printf("%s\n",buf);
        free(buf);
    }

    // Empty stack
    else{
        fprintf(stderr, "Error cannot print empty stack\n");
        exit(0);
    }
    return 0;
}



/*
*   Helper function that checks if top two stack items are valid for operations
*/
int checkStackTop(bn_stack stack){
    int top = stack->top;
    if (top < 2){
        fprintf(stderr, "Error need at least two items on stack to perform operation\n");
        exit(0);
    }
    if ((stack->array[stack->top] == NULL) || (stack->array[top-1] == NULL)){
        fprintf(stderr, "Error NULL value found in stack\n");
        exit(0);
    }
    return 1;
}

/*
*   Swaps positions of top stack item with item below
*/
int swap(bn_stack stack){
    if (checkStackTop(stack) != 1){
        fprintf(stderr, "Error not enough numbers to swap\n" );
        exit(0);
    }
        bn_t temp = stack->array[stack->top];
        stack->array[stack->top] = stack->array[stack->top-1];
        stack->array[stack->top-1] = temp;
    return 0;
}
/*
*   Prints each item in the stack without altering values
*/
int dump(bn_stack stack){
    if (stack->top > 0){
        // for (size_t i = stack->top; i>0; i--){
        for (size_t i = 0; i < stack->top; i++){
            bn_t bn = stack->array[stack->top - i];
            if (bn == NULL){
                fprintf(stderr, "Error null entry in stack\n");
                exit(0);
            }
            //Get size of buffer
            int bufSize = bn_toString(bn, "", 0);
            char * buf = (char*) malloc(bufSize * sizeof(char));
            bn_toString(bn, buf, bufSize);
            printf("%s\n",buf);
            free(buf);
        }    
    }
    return 0;
}

/*
*  Clears the stack
*/
int clear(bn_stack stack){
    free(stack->array);
    stack->array = (bn_t *) calloc(10, sizeof(bn_t));
    stack->size = 10;
    stack->top = 0;
    return 0;
}

/*
*  Duplicates item on top of stack and places it on top
*/ 
int dup(bn_stack stack){
    // Requires item on stack
    if (stack->top > 0){
        // Quit if null
        if (stack->array[stack->top] == NULL){
            fprintf(stderr, "NULL item in stack\n");
            exit(0);
        }
        bn_t bn = bn_alloc();
        bn_t zero = bn_alloc();
        bn_add(bn, zero, stack->array[stack->top]);
        
        bn_free(zero);

        push(bn, stack);
        return 0;
    }
    else{
        fprintf(stderr, "Error cannot duplicate an empty stack\n");
        exit(0);
    }
    return 0;
}

/*
*   Multiplies top stack result with second result 
*   Places result on top of stack
*/
int mult(bn_stack stack){
    int top = stack->top;
    if (checkStackTop(stack) !=1){
        return -1;
    }
    bn_t ans = bn_alloc();

    if (bn_mul(ans, stack->array[top], stack->array[top-1])==-1){
        fprintf(stderr, "Error multiplying\n");
        return -1;
    }
    pop(stack);
    pop(stack);
    push(ans, stack);
    return 0;
}
/*
*   Adds top stack result to second result 
*   Places result on top of stack
*/
int add(bn_stack stack){
    int top = stack->top;
    if (checkStackTop(stack) !=1){
        return -1;
    }
    bn_t ans = bn_alloc();
    if (bn_add(ans, stack->array[top], stack->array[top-1])==-1){
        fprintf(stderr, "Error adding\n");
        return -1;
    }
    pop(stack);
    pop(stack);
    push(ans, stack);
    return 0;
}
/*
*   Subtracts top stack result from second stack result 
*   Places result on top of stack
*/
int sub(bn_stack stack){
    int top = stack->top;
    if (checkStackTop(stack) !=1){
       return -1;
    }
    bn_t ans = bn_alloc();
    if (bn_sub(ans, stack->array[top-1], stack->array[top]) == -1){
        fprintf(stderr, "Error subtracting\n");
        return -1;
    }
    pop(stack);
    pop(stack);
    push(ans, stack);
    return 0;
}
/**
*   Reads a command string and performs the appropriate action
*/
int readString(char* str, bn_stack stack){

    if (strcmp("*", str ) == 0){
        return mult(stack);
    }
    if (strcmp("+", str)== 0){
        return add(stack);
    }
    if (strcmp("-", str) == 0){
        return sub(stack);
    }
    if (strcmp("dup", str) == 0){
        return dup(stack);
    }
    if (strcmp("pop", str) == 0){
        return pop(stack);
    }
    if (strcmp("print", str) == 0){
        return print(stack);
    }
    if (strcmp("swap", str) == 0){
        return swap(stack);
    }
    if (strcmp("dump", str) == 0){
        return dump(stack);
    }
    if (strcmp("clear", str) == 0){
        return clear(stack);
    }
    else{
        bn_t bn = bn_alloc();
        if (bn_fromString(bn, str) != 0){
            bn_free(bn);
            fprintf(stderr, "Error unrecognised string %s\n",str);
            exit(0);
        }
        int result  = push(bn, stack);
        return result;
    }
}
/*
*   Reads input from file and converts it to a string
*/
static char * getInputString(FILE * pf, size_t size){
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, size);
    if (!str){
        return str;
    }
    while (EOF!=(ch = fgetc(pf))){
        str[len++] = ch;
        if(len == size){
            str = realloc(str, size+=16);
            if(!str){
                return str;
            }
        }
    }
    str[len++] = '\0';
    return realloc(str, len);
}

int main(){

    bn_stack stack = bn_stackAlloc();    

    char * str;
    str = getInputString(stdin, 1)   ;
    char * pch;

    // Parse the string
    pch = strtok (str, " \t\n\v\f\r");

    while (pch != NULL)
    {
        if (readString(pch, stack) == -1){
            fprintf(stderr, "Error detected\n");
            exit(0);
    }
    pch = strtok (NULL, " \t\n\v\f\r");
    }

    free(stack->array);
    free(stack);
    free(pch);
    free(str);

    return 0;
}
