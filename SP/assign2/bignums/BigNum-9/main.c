#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "bn.h"

//check for empty
//new node
//push
//pop
//find length

struct stack{
    bn_t bn;
    struct stack* next;
};

struct stack* head = NULL;

int is_empty(){
    if (head == NULL) {
        return 1;
    }
    return 0;
}

struct stack* create_node(bn_t a){
    struct stack* node = (struct stack*) malloc(sizeof(struct stack));
    node->bn = a;
    node->next = NULL;
    return node;
}

void push(bn_t a){
    struct stack* node = create_node(a);
    node->next = head;
    head = node;
}

bn_t pop(){
    if (is_empty()) {
        return NULL;
    }
    struct stack* node = head;
    head = head->next;
    bn_t res = node->bn;
    free(node);
    return res;
}

int dump_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t bn;
    struct stack* node = head;
    while (node != NULL) {
        bn = node->bn;
        printtostring(bn);
        node = node->next;
    }
    return 0;
}

int print_stack_top(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t bn;
    bn = head->bn;
    printtostring(bn);
    return 0;
}

int clear_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 0;
    }
    bn_t bn;
    while (head != NULL) {
        bn = pop();
        bn_free(bn);
    }
    return 0;
}

int swap_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t a = pop();
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t b = pop();
    push(a);
    push(b);
    return 0;
}

int dup_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t a = pop();
    bn_t b = bn_alloc();
    bn_copy(b,a);
    push(a);
    push(b);
    return 0;
}

int add_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t a = pop();
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t b = pop();
    bn_t res = bn_alloc();
    bn_add(res,a,b);
    push(res);
    return 0;
}

int sub_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t a = pop();
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t b = pop();
    bn_t res = bn_alloc();
    bn_sub(res,b,a);
    push(res);
    return 0;
}

int mul_stack(){
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t a = pop();
    if (is_empty()) {
        // printf("Stack is empty\n");
        return 1;
    }
    bn_t b = pop();
    bn_t res = bn_alloc();
    bn_mul(res,a,b);
    push(res);
    return 0;
}

void append(char* res, char a){
    char *tmp;
    tmp = (char*) malloc(2);
    tmp[0] = a;
    tmp[1] = '\0';
    strcat(res, tmp);
    free(tmp);
}

void error(){
    fprintf(stderr, "Error\n");
    exit(-1);
}

int main() {
    char c;
    char token[USHRT_MAX];
    int digits = 1;
    while (1) {
        c = getchar();
        if (!isdigit(c) && c!= EOF && c != '\n' && !isspace(c)) {
            digits = 0;
        }
        if (c == EOF) {
            return 0;
        }else if (isspace(c) || c == '\n' || c == EOF) {
            if (strcmp(token,"+") == 0) {
                // printf("add\n");
                if ( add_stack() ) { error(); }
            }else if (strcmp(token,"-") == 0) {
                // printf("sub\n");
                if ( sub_stack() ) { error(); }
            }else if (strcmp(token,"*") == 0) {
                // printf("mul\n");
                // dump_stack();
                if ( mul_stack() ) { error(); }
            }else if (strcmp(token,"dup") == 0) {
                // printf("dup\n");
                if ( dup_stack() ) { error(); }
            }else if (strcmp(token,"pop") == 0) {
                // printf("pop\n");
                pop();
            }else if (strcmp(token,"print") == 0) {
                // printf("print\n");
                print_stack_top();
            }else if (strcmp(token,"swap") == 0) {
                // printf("swap\n");
                swap_stack();
            }else if (strcmp(token,"dump") == 0) {
                // printf("dump\n");
                dump_stack();
            }else if (strcmp(token,"clear") == 0) {
                // printf("clear\n");
                clear_stack();
            }else if (digits && strlen(token) > 0) {
                // printf("number: %s\n", token);
                bn_t bn = bn_alloc();
                bn_fromString(bn,token);
                push(bn);
            }else if (!digits) {
                printf("? %s\n", token);
                fprintf(stderr, "Error\n");
                return -1;
            }
            memset(token, 0, sizeof(token));
            digits = 1;
        }else if (c != '\n') {
            append(token,c);
            // printf("%s\n", token);
        }else{
            printf("| %c %s\n", c, token);
            fprintf(stderr, "Error\n");
            return -1;
        }
    }
    return 0;
}
