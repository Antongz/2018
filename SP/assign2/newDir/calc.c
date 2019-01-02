//###- The code below is written by Yi-Ting, Hsieh(a1691807) -###

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "bn.c"

#define buffer_size 32767
//#define buffer_size 2147483647
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
//    return !root;
    if (root != NULL)
        return 0;
    return 1;
}

int getStackLength(struct StackNode** root) {
    if (isEmpty(*root)) return 0;
    struct StackNode* temp = *root;
    int length = 1;
    while (temp->next != NULL)
        length++;

    return length;
}

void push(struct StackNode** root, bn_t bn) {
    struct StackNode* stackNode = newNode(bn);
    stackNode->next = *root;
    *root = stackNode;
}

bn_t pop(struct StackNode** root) {
    if (isEmpty(*root)) { status = -1; return NULL; }
    struct StackNode* temp = *root;
    *root = (*root)->next;
    bn_t popped = temp->bn;
    free(temp);
    return popped;
}

void dup(struct StackNode** root) {
    if (isEmpty(*root)) { status = -1; return; }
    bn_t a = pop(root);
    bn_t temp = bn_alloc();
    if (temp == NULL) { status = -1; return; }
    if (copyBN(temp, a) == -1) { status = -1; return; }
    push(root, a);
    push(root, temp);
}

void swap(struct StackNode** root) {
    bn_t a = pop(root);
    if (a == NULL) return;
    bn_t b = pop(root);
    if (b == NULL) return;
    push(root, a);
    push(root, b);
}

void clear(struct StackNode** root) {
    bn_t temp;
    while ((*root) != NULL) { temp = pop(root); bn_free(temp); }
}

void printStack(struct StackNode** root) {
    if (!isEmpty(*root)) {
        bn_t temp = (*root)->bn;
        char* buffer1 = malloc(4096);
        size_t sz  = bn_toString(temp, buffer1, 0) + 1;
        free(buffer1);
        char* buffer = malloc(sz);
        bufferStatus = bn_toString(temp, buffer, sz);
        if (bufferStatus == -1) { status = -1; return; }
        printf("%s\n", buffer);
        free(buffer);
    }
}

void dumpCall(struct StackNode** root) {
    bn_t temp;
    struct StackNode* tempNode = *root;

    while (tempNode != NULL) {
        temp = tempNode->bn;
        char* buffer1 = malloc(4096);
        size_t sz  = bn_toString(temp, buffer1, 0) + 1;
        free(buffer1);
        char* buffer = malloc(sz);
        bufferStatus = bn_toString(temp, buffer, sz);
        if (bufferStatus == -1) { status = -1; return; }
        printf("%s\n", buffer);
        free(buffer);
        tempNode = tempNode->next;
    }
}

int parse(char* buffer) {
    size_t sz = strlen(buffer);
    size_t i = 0;

    while (i < sz && status == 0) {
        if (isdigit(buffer[i])) {
            size_t end = i;
            while (isdigit(buffer[++end])) {}
            char temp = buffer[end];
            buffer[end] = 0;

            bn_t bn = bn_alloc();
            bn_fromString(bn, buffer + i);
            push(&root, bn);

            i = end;
            buffer[end] = temp;
        }

        else if (buffer[i] == '+') {
            i++;
            bn_t operand1, operand2;
            bn_t result = bn_alloc();
            if ((operand2 = pop(&root)) == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_add(result, operand1, operand2)) == -1) { return -1; }
            push(&root, result);
            bn_free(operand2);
            bn_free(operand1);
        }
        else if (buffer[i] == '-') {
            i++;
            bn_t operand1, operand2;
            bn_t result = bn_alloc();
            if ((operand2 = pop(&root)) == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_sub(result, operand1, operand2)) == -1) { return -1; }
            push(&root, result);
            bn_free(operand2);
            bn_free(operand1);
        }
        else if(buffer[i] == '*') {
            i++;
            bn_t operand1, operand2;
            bn_t result = bn_alloc();
            if ((operand2 = pop(&root)) == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_mul(result, operand1, operand2)) == -1) { return -1; }
            push(&root, result);
            bn_free(operand2);
            bn_free(operand1);
        }
        else if(buffer[i] == '/') {
            i++;
            bn_t operand1, operand2;
            bn_t quo = bn_alloc();
            if (quo == NULL) { return -1; }
            bn_t rem = bn_alloc();
            if (rem == NULL) { return -1; }
            if ((operand2 = pop(&root)) == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_div(quo, rem, operand1, operand2)) == -1) { return -1; }
            // push back quot
            push(&root, quo);
            // push back rem
            push(&root, rem);
            bn_free(operand1);
            bn_free(operand2);
        }
        else if (strncmp(buffer + i, "sqr", 3) == 0) {
            i += 3;
            bn_t operand1;
            bn_t res = bn_alloc();
            if (res == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_sqr(res, operand1)) == -1) { return -1; }
            push(&root, res);
            bn_free(operand1);
        }

        else if(strncmp(buffer + i, "modexp", 6) == 0) {
            i += 6;
            bn_t operand1, operand2, operand3;
            bn_t res = bn_alloc();
            if (res == NULL) { return -1; }
            if ((operand3 = pop(&root)) == NULL) { return -1; }
            if ((operand2 = pop(&root)) == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_modexp(res, operand1, operand2, operand3)) == -1) { return -1; }
            // push back quot
            push(&root, res);
            bn_free(operand1);
            bn_free(operand2);
            bn_free(operand3);
        }

        else if (strncmp(buffer + i, "dup", 3) == 0) {
            i += 3;
            dup(&root);
        } else if (strncmp(buffer + i, "pop", 3) == 0) {
            i += 3;
            bn_t temp = pop(&root);
            bn_free(temp);
        } else if (strncmp(buffer + i, "print", 5) == 0) {
            i += 5;
            printStack(&root);
        } else if (strncmp(buffer + i, "swap", 4) == 0) {
            i += 4;
            swap(&root);
        } else if (strncmp(buffer + i, "dump", 4) == 0) {
            i += 4;
            dumpCall(&root);
        } else if (strncmp(buffer + i, "clear", 5) == 0) {
            clear(&root);
        } else {
            i++;
        }
    }

    return 0;
}

//int main() {
//    bufferLength = buffer_size;

//    char buffer[2048];

//    while(fgets(buffer, 2048, stdin) != NULL){
//        if (parse(buffer) != 0) {
//            status = -1;
//            break;
//        }
//    }

//    if (status == -1) { fprintf(stderr, "Error\n"); clear(&root); return -1; }
//    return 0;
//}
