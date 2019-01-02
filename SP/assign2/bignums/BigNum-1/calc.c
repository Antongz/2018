#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "bn.h"

#define buffer_size 32767
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
        bufferStatus = bn_toString((*root)->bn, printBuffer, bufferLength);
        while (bufferStatus > 0) {
            bufferLength = bufferStatus;
            printBuffer = (char*) realloc (printBuffer, bufferLength + 1);
            bufferStatus = bn_toString((*root)->bn, printBuffer, bufferLength);
        }
        if (bufferStatus == -1) { status = -1; return; }
        printf("%s\n", printBuffer);
    }
}

void dump(struct StackNode** root) {
    bn_t temp;
    struct StackNode* tempNode = *root;

    while (tempNode != NULL) {
        temp = tempNode->bn;
        bufferStatus = bn_toString(temp, printBuffer, bufferLength);
        while (bufferStatus > 0) {
            bufferLength = bufferStatus;
            printBuffer = (char*) realloc (printBuffer, bufferLength + 1);
            bufferStatus = bn_toString(temp, printBuffer, bufferLength);
        }
        if (bufferStatus == -1) { status = -1; return; }
        printf("%s\n", printBuffer);
        tempNode = tempNode->next;
    }
}

int main() {
    bufferLength = buffer_size;
    bufferStatus = 0;
    printBuffer = (char*) realloc (printBuffer, bufferLength);

    char c[buffer_size];
    memset(c, 0, sizeof (c));
    char s;
    char str[2];
    bn_t operand1, operand2;
    bool isAllDigits = true;

    while (status != -1) {
        s = getchar();
        if ((!isdigit(s)) && (s != EOF && s != '\n' && !isspace(s)))
            isAllDigits = false;
        if (isspace(s) || (s == '\n') || (s == EOF)) {

            if (strcmp(c, "+") == 0) {
                if ((operand2 = pop(&root)) == NULL) { status = -1; break; }
                if ((operand1 = pop(&root)) == NULL) { status = -1; break; }
                if ((status = bn_add(operand1, operand1, operand2)) == -1) { status = -1; break; }
                push(&root, operand1);
            }
            else if (strcmp(c, "-") == 0) {
                if ((operand2 = pop(&root)) == NULL) { status = -1; break; }
                if ((operand1 = pop(&root)) == NULL) { status = -1; break; }
                if ((status = bn_sub(operand1, operand1, operand2)) == -1) { status = -1; break; }
                push(&root, operand1);
            }
            else if (strcmp(c, "*") == 0) {
                if ((operand2 = pop(&root)) == NULL) { status = -1; break; }
                if ((operand1 = pop(&root)) == NULL) { status = -1; break; }
                if ((status = bn_mul(operand1, operand1, operand2)) == -1) { status = -1; break; }
                push(&root, operand1);
            }
            else if (strcmp(c, "dup") == 0)
                dup(&root);
            else if (strcmp(c, "pop") == 0)
                pop(&root);
            else if (strcmp(c, "print") == 0)
                printStack(&root);
            else if (strcmp(c, "swap") == 0)
                swap(&root);
            else if (strcmp(c, "dump") == 0)
                dump(&root);
            else if (strcmp(c, "clear") == 0)
                clear(&root);
            else if (strlen(c) > 0 && isAllDigits) {
                bn_t temp = bn_alloc();
                if (temp == NULL) { status = -1; break; }
                if (bn_fromString(temp, c) == -1) { status = -1; break; }
                push(&root, temp);
            }
            else if (!isAllDigits) { status = -1; break; }

            memset(c, 0, sizeof (c));
            isAllDigits = true;
        }
        else if (s != '\n'){
            memcpy(str, &s, sizeof(char));
            strcat(c, str);
        }
        if (s == EOF) return 0;
    }

    if (status == -1) { fprintf(stderr, "Error\n"); return -1; }
    return 0;
}
