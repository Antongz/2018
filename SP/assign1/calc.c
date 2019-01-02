/*
 * =====================================================================================
 *
 *       Filename:  calculator.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/16/2018 01:38:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "bn.h"

#define buffer_size 1000
extern int errno;
char *printBuffer;
int bufferLength;
int bufferStatus;

static int status = 0;


// A structure to represent a stack
struct StackNode {
    bn_t bn;
    struct StackNode* next;
};

static struct StackNode* root = NULL;

struct StackNode* newNode(bn_t this_bn) {
    struct StackNode* stackNode = (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->bn = this_bn;
    stackNode->next = NULL;

    return stackNode;
}


int isEmpty(struct StackNode* root) {
    return !root;
}

int getStackLength(struct StackNode* root) {
    if (isEmpty(root)) return 0;

    struct StackNode* temp = root;
    int length = 1;

    while (temp->next != NULL) {
        length++;
    }

    return length;
}


void push(struct StackNode** root, bn_t bn) {
    struct StackNode* stackNode = newNode(bn);
    stackNode->next = *root;
    *root = stackNode;
}

bn_t pop(struct StackNode** root) {
    if (isEmpty(*root)) {
        status = -1;
        return NULL;
    }
    struct StackNode* temp = *root;
    *root = (*root)->next;
    bn_t popped = temp->bn;
    free(temp);

    return popped;
}

// Copies the value of the bignum at the top of the stack and pushes the 
// duplicate value into stack
void dup(struct StackNode** root) {
    if (isEmpty(*root)) {


        return;
    }
    bn_t a = pop(root);

    bn_t temp = bn_alloc();
    if (temp == NULL) {
        status = -1;
        return;
    }
    if (copyBN(temp, a) == -1) {
        status = -1;
        return;
    }
    push(root, a);
    push(root, temp);

}

// Swaps the order of the two bignumber at the top of the stack
void swap(struct StackNode** root) {
    bn_t a = pop(root);
    bn_t b = pop(root);

    push(root, a);
    push(root, b);
}

// Clears the stack.
void clear(struct StackNode** root) {
    bn_t temp;

    while ((*root) != NULL) {
        temp = pop(root);
        bn_free(temp);
    }
}

// Prints the value of the bignum at the top of the stack followed by a newline('\n')
void printStack(struct StackNode** root) {

    if (!isEmpty(*root)) {
        bufferStatus = bn_toString((*root)->bn, printBuffer, bufferLength);
        while (bufferStatus != 0) {
            bufferLength = bufferStatus;
            printBuffer = (char*) realloc (printBuffer, bufferLength + 1);
            bufferStatus = bn_toString((*root)->bn, printBuffer, bufferLength);
        }
        printf("%s\n", printBuffer);
    }
}

// Prints the contents of the stack, one bignum per line, starting from the stack top.
// The stack is not modified.
void dump(struct StackNode** root) {
    bn_t temp;
    struct StackNode* tempNode = *root;

    while (tempNode != NULL) {
        temp = tempNode->bn;
        bufferStatus = bn_toString(temp, printBuffer, bufferLength);
        while (bufferStatus != 0) {
            bufferLength = bufferStatus;
            printBuffer = (char*) realloc (printBuffer, bufferLength + 1);
            bufferStatus = bn_toString(temp, printBuffer, bufferLength);
        }
        printf("%s\n", printBuffer);
        tempNode = tempNode->next;
    }
}

int main() {
    bufferLength = buffer_size;
    bufferStatus = 0;
    printBuffer = (char*) realloc (printBuffer, bufferLength);
    char c[buffer_size];
    printf("HERE\n");

    memset(c, 0, buffer_size);
    char s;
    char str[2];
    bool canExist = false;

    bn_t operand1, operand2;

    while (status != -1 || !canExist) {
        s = getchar();
        
        if (isspace(s) || (s == '\n')) {

            if (strcmp(c, "+") == 0) {
                operand2 = pop(&root);
                operand1 = pop(&root);
                status = bn_add(operand1, operand1, operand2);
                push(&root, operand1);
            }
            else if (strcmp(c, "-") == 0) {
                operand2 = pop(&root);
                operand1 = pop(&root);
                status = bn_sub(operand1, operand1, operand2);
                push(&root, operand1);
            }
            else if (strcmp(c, "*") == 0) {
                operand2 = pop(&root);
                operand1 = pop(&root);
                status = bn_mul(operand1, operand1, operand2);
                push(&root, operand1);
            }
            else if (strcmp(c, "dup") == 0)
                dup(&root);
            else if (strcmp(c, "pop") == 0)
                pop(&root);
            else if (strcmp(c, "print") == 0) {
                printStack(&root);
                if ((getStackLength(root) == 1) && (s == '\n')) canExist = true;
            } else if (strcmp(c, "swap") == 0)
                swap(&root);
            else if (strcmp(c, "dump") == 0)
                dump(&root);
            else if (strcmp(c, "clear") == 0) {
                clear(&root);
                canExist = false;
            } else if (strlen(c) > 0) {
                bn_t temp = bn_alloc();
                bn_fromString(temp, c);
                push(&root, temp);
            }

            memset(c, 0, buffer_size);
        }

        else if (s != '\n'){
            memcpy(str, &s, sizeof(char));
            strcat(c, str);
        }

        if (canExist)
            return 0;
    }

    if (status == -1) {
//        errnum = errno;
        fprintf(stderr, "Error\n");
    }

    return 0;
}


