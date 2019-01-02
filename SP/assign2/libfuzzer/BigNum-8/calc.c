#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
// #include <limits.h>

// #include "calc.h"
#include "bn.h"
#include "bn.c"

//Structure representing a stack
struct Stack {
    int top;
    int size;
    bn_t* array;
};

static int str_len(const char* s) {
    int count = 0;
    while(s[count] != '\0') {
        count++;
    }
    return count;
}


//prints top of stack
int print_top(struct Stack* s) {
    if(s->top == -1) {
        return 1;
    }
    char* buf = (char*) malloc(100 * sizeof(char));
    if(buf == NULL) {
        fprintf(stderr, "Error: NULL assignment\n");
        return 1;
    }
    int required = bn_toString(s->array[s->top], buf, 100);
    if(required > 0) {
        buf = (char*) realloc(buf, required*required);
        required = bn_toString(s->array[s->top], buf, required);
        if(buf == NULL) {
            fprintf(stderr, "Error: NULL assignment\n");
            return 1;
        }
    }
    printf("%s\n", buf);
    free(buf);
    return 0;
}
 

//Create a new stack with input size
struct Stack* new_stack(unsigned size) {
    struct Stack* s = (struct Stack*) malloc(sizeof(struct Stack));
    if(s == NULL) {
        fprintf(stderr, "Error: NULL assignment\n");
    }
    s->size = size;
    s->top = -1;
    s->array = (bn_t*) malloc(s->size * sizeof(bn_t*));
    if(s->array == NULL) {
        fprintf(stderr, "Error: NULL assignment\n");
    }
    for(int i=0 ; i<size ; i++) {
        s->array[i] = bn_alloc();
        s->array[i]->bn_len = 0;
    }
    return s;
}


//Pushes an item onto the stack from string
int push(struct Stack* s, char* num) {
	//If Stack is full
    if(s->top == s->size) {
        fprintf(stderr, "Error: stack full\n");
		return 1;
	}
    s->top++;
    bn_fromString(s->array[s->top], num);
    return 0;
}

//Pushes an item onto the stack from bn_t
int push_bn(struct Stack* s, bn_t num) {
    if(s->top == s->size) {
        fprintf(stderr, "Error: stack full\n");
        return 1;
    }
    s->top++;
    s->array[s->top] = num;
    return 0;
}
 

//Pops an item from the stack
int pop(struct Stack* s) {
	//If Stack is empty
    if(s->top == -1) {
        fprintf(stderr, "Error: stack empty\n");
        return 1;
	}
    s->top--;
    return 0;
}

//Pops an item from the stack as bn_t
bn_t pop_bn(struct Stack* s) {
    //If Stack is empty
    if(s->top == -1) {
        fprintf(stderr, "Error: stack empty\n");
        return NULL;
    }
    return s->array[s->top--];
}

//Copies value at top of stack and pushes it
int dup(struct Stack* s) {
    if(s->top == s->size) {
        fprintf(stderr, "Error: stack full\n");
        return 1;
    }
    if(s->top == -1) {
        fprintf(stderr, "Error: stack empty\n");
        return 1;
    }

    s->top++;
    memcpy(s->array[s->top], s->array[s->top-1], sizeof(s->array[s->top-1]) * sizeof(bn_t*));
    return 0;
}


//Swap the order of the top two numbers
int swap(struct Stack* s) {
    if(s->top < 1) {
        fprintf(stderr, "Error: less than one element on stack\n");
        return 1;
    }
    bn_t t1 = pop_bn(s);
    bn_t t2 = pop_bn(s);
    push_bn(s, t1);
    push_bn(s, t2);
    return 0;
}


//Print contents of stack
int dump(struct Stack* s) {
    char* buf = (char*) malloc(100 * sizeof(char));
    if(buf == NULL) {
        fprintf(stderr, "Error: NULL assignment\n");
        return 1;
    }
    for(int i=s->top ; i>=0 ; i--) {
        int required = bn_toString(s->array[i], buf, 100);
        if(required > 0) {
            buf = (char*) realloc(buf, required*required);
            required = bn_toString(s->array[i], buf, required);
            if(buf == NULL) {
                fprintf(stderr, "Error: NULL assignment\n");
                return 1;
            }
        }
        printf("%s\n", buf);
    }
    free(buf);
    return 0;
}


//Applies an operator to the top two elements
int operator(struct Stack* s, char c) {
    if(s->top < 1) {
        fprintf(stderr, "Error: less than one element on stack\n");
        return 1;
    }
    bn_t b1 = pop_bn(s);
    bn_t b2 = pop_bn(s);
    bn_t result = bn_alloc();
    if(c == '+') {
        bn_add(result, b1, b2);
    }
    else if(c == '-') {
        bn_sub(result, b2, b1);
    }
    else if(c == '*') {
        bn_mul(result, b1, b2);
    }
    else {
        fprintf(stderr, "Error: invalid operator. Note: THIS SHOULD NEVER HAPPEN\n");
        return 1;
    }
    if(push_bn(s, result)) { return 1; }
    return 0;
}

//Clears the stack
int clear_stack(struct Stack* s) {
    s->top = -1;
    return 0;
}


//empties string of size n
void clear_string(char* str, int n) {
    for(int i=0 ; i<n ; i++) {
        str[i] = '\0';
    }
}


//Reads words from input and does their operation
int parse_input(struct Stack* s, char* input) {
    int n = str_len(input);
    char word[n];
    int word_size = 0;
    int notnum = 0;
    for(int i=0 ; i<n ; i++) {
        if(isspace(input[i])) {
            word[word_size] = '\0';
            notnum = 0;
            //Determine word type
            for(int j=0 ; j<word_size ; j++) {
                //Checking for non-numeric characters
                if((word[j] < 48) || (word[j] > 57)) {
                    notnum = 1;
                } 
            }
            //If it is a number
            if((!notnum) && (word_size > 0)) {
                if(push(s, word)) { return 1; }
            }
            //Checking for operator
            else if(((word[0] == '*') || (word[0] == '+') || (word[0] == '-')) 
                && (word_size == 1)) {
                if(operator(s, word[0])) { return 1; }
            }
            //dup
            else if(((word[0] == 'd') && (word[1] == 'u') && (word[2] == 'p')) 
                && (word_size == 3)) {
                if(dup(s)) { return 1; }
            }
            //pop
            else if(((word[0] == 'p') && (word[1] == 'o') && (word[2] == 'p')) 
                && (word_size == 3)) {
                if(pop(s)) { return 1; }
            }
            //swap
            else if(((word[0] == 's') && (word[1] == 'w') && (word[2] == 'a') && (word[3] == 'p')) 
                && (word_size == 4)) {
                if(swap(s)) { return 1; }
            }
            //dump
            else if(((word[0] == 'd') && (word[1] == 'u') && (word[2] == 'm') && (word[3] == 'p')) 
                && (word_size == 4)) {
                if(dump(s)) { return 1; }
            }
            //print
            else if(((word[0] == 'p') && (word[1] == 'r') && 
                (word[2] == 'i') && (word[3] == 'n') && (word[4] == 't')) 
                && (word_size == 5)) {
                if(print_top(s)) { return 1; }
            }
            //clear
            else if(((word[0] == 'c') && (word[1] == 'l') && 
                (word[2] == 'e') && (word[3] == 'a') && (word[4] == 'r')) 
                && (word_size == 5)) {
                if(clear_stack(s)) { return 1; }
            }
            //Invalid word
            else if (word_size != 0) {
                fprintf(stderr, "Error: '%s' is not a valid word type\n", word);
                return 1;
            }

            word_size = 0;
        }
        else {
            word[word_size++] = input[i];
        }
    }

    return 0;
}


//int main(int argc, const char* argv[]) {
//    struct Stack* stack = new_stack(500);

//    char input[5000];
//    while(!feof(stdin)) {
//        input[0] = '\0';
//        fgets(input, 5000, stdin);
//        if(parse_input(stack, input)) {
//            return 1;
//        }
//    }
 
//    return 0;
//}
