//###- The code below is written by Yi-Ting, Hsieh(a1691807) -###

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include "bn.h"
#include "ybn/ybn.h"

int bufferStatus;
static int status = 0;


// A structure to represent a stack
struct StackNode {
    bn_t bn;
    struct StackNode* next;
};

struct YBNNode {
    ybn_t bn;
    struct YBNNode* next;
};

static struct StackNode* root = NULL;
static struct YBNNode* ybnRoot = NULL;


struct StackNode* newNode(bn_t this_bn) {
    struct StackNode* stackNode = (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->bn = this_bn;
    stackNode->next = NULL;
    return stackNode;
}

struct YBNNode* newYBNNode(ybn_t this_bn) {
    struct YBNNode* stackNode = (struct YBNNode*) malloc(sizeof(struct YBNNode));
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

int isYBNEmpty(struct YBNNode* ybnRoot) {
    if (ybnRoot != NULL)
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

void pushYBN(struct YBNNode** ybnRoot, ybn_t bn) {
    struct YBNNode* stackNode = newYBNNode(bn);
    stackNode->next = *ybnRoot;
    *ybnRoot = stackNode;
}

bn_t pop(struct StackNode** root) {
    if (isEmpty(*root)) { status = -1; return NULL; }
    struct StackNode* temp = *root;
    *root = (*root)->next;
    bn_t popped = temp->bn;
    free(temp);
    return popped;
}

ybn_t popYBN(struct YBNNode** ybnRoot) {
    if (isYBNEmpty(*ybnRoot)) { status = -1; return NULL; }
    struct YBNNode* temp = *ybnRoot;
    *ybnRoot = (*ybnRoot)->next;
    ybn_t popped = temp->bn;
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

void dupYBN(struct YBNNode** root) {
    if (isYBNEmpty(*root)) { status = -1; return; }
    ybn_t a = popYBN(root);
    ybn_t temp = ybn_alloc();
    if (temp == NULL) { status = -1; return; }
    if (copyYBN(temp, a) == -1) { status = -1; return; }
    pushYBN(root, a);
    pushYBN(root, temp);
}

void swap(struct StackNode** root) {
    bn_t a = pop(root);
    if (a == NULL) return;
    bn_t b = pop(root);
    if (b == NULL) return;
    push(root, a);
    push(root, b);
}

void swapYBN(struct YBNNode** root) {
    ybn_t a = popYBN(root);
    if (a == NULL) return;
    ybn_t b = popYBN(root);
    if (b == NULL) return;
    pushYBN(root, a);
    pushYBN(root, b);
}

void clear(struct StackNode** root) {
    bn_t temp;
    while ((*root) != NULL) { temp = pop(root); bn_free(temp); }
}

void clearYBN(struct YBNNode** root) {
    ybn_t temp;
    while ((*root) != NULL) { temp = popYBN(root); ybn_free(temp); }
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

void printYBNStack(struct YBNNode** root) {
    if (!isYBNEmpty(*root)) {
        ybn_t temp = (*root)->bn;
        char* buffer1 = malloc(4096);
        size_t sz  = ybn_toString(temp, buffer1, 0) + 1;
        free(buffer1);
        char* buffer = malloc(sz);
        bufferStatus = ybn_toString(temp, buffer, sz);
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

void dumpYBNCall(struct YBNNode** root) {
    ybn_t temp;
    struct YBNNode* tempNode = *root;

    while (tempNode != NULL) {
        temp = tempNode->bn;
        char* buffer1 = malloc(4096);
        size_t sz  = ybn_toString(temp, buffer1, 0) + 1;
        free(buffer1);
        char* buffer = malloc(sz);
        bufferStatus = ybn_toString(temp, buffer, sz);
        if (bufferStatus == -1) { status = -1; return; }
        printf("%s\n", buffer);
        free(buffer);
        tempNode = tempNode->next;
    }
}

int comparedBNYBN(bn_t bn, ybn_t ybn) {

    //bn
    char* buffer1 = malloc(4096);
    size_t sz  = bn_toString(bn, buffer1, 0) + 1;
    free(buffer1);
    char* buffer = malloc(sz);
    if (bn_toString(bn, buffer, sz) == -1) { status = -1; return -1; }

    //ybn
    char* buffer2 = malloc(4096);
    size_t sz2  = bn_toString(bn, buffer2, 0) + 1;
    free(buffer2);
    char* buffer3 = malloc(sz2);
    if (ybn_toString(ybn, buffer3, sz2) == -1) { status = -1; return -1; }

    // Compare the output.
    if (strcmp(buffer, buffer3) != 0) {
        printf("=======Detected Different outcome===========\n");
        printf("Bn is: %s\nYBN is: %s\n", buffer, buffer3);
        free(buffer);
        free(buffer3);

        abort();
    }

    free(buffer);
    free(buffer3);

    return 0;
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

            ybn_t ybn = ybn_alloc();
            ybn_fromString(ybn, buffer + i);

            if (comparedBNYBN(bn, ybn) != 0)
                return -1;

            push(&root, bn);
            pushYBN(&ybnRoot, ybn);

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

            //ybn
            ybn_t ybn1, ybn2;
            ybn_t resultYBN = ybn_alloc();
            if ((ybn2 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((ybn1 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((status = ybn_add(resultYBN, ybn1, ybn2)) == -1) { return -1; }
            pushYBN(&ybnRoot, resultYBN);
            ybn_free(ybn1);
            ybn_free(ybn2);


            if (comparedBNYBN(result, resultYBN) != 0)
                return -1;
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

            //ybn
            ybn_t ybn1, ybn2;
            ybn_t resultYBN = ybn_alloc();
            if ((ybn2 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((ybn1 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((status = ybn_sub(resultYBN, ybn1, ybn2)) == -1) { return -1; }
            pushYBN(&ybnRoot, resultYBN);
            ybn_free(ybn1);
            ybn_free(ybn2);

            if (comparedBNYBN(result, resultYBN) != 0)
                return -1;
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

            //ybn
            ybn_t ybn1, ybn2;
            ybn_t resultYBN = ybn_alloc();
            if ((ybn2 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((ybn1 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((status = ybn_mul(resultYBN, ybn1, ybn2)) == -1) { return -1; }
            pushYBN(&ybnRoot, resultYBN);
            ybn_free(ybn1);
            ybn_free(ybn2);

            if (comparedBNYBN(result, resultYBN) != 0)
                return -1;
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

            //ybn
            ybn_t ybn1, ybn2;
            ybn_t ybnquo = ybn_alloc();
            if (ybnquo == NULL) { return -1; }
            ybn_t ybnrem = ybn_alloc();
            if (ybnrem == NULL) { return -1; }
            if ((ybn2 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((ybn1 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((status = ybn_div(ybnquo, ybnrem, ybn1, ybn2)) == -1) { return -1; }
            // push back quot
            pushYBN(&ybnRoot, ybnquo);
            // push back rem
            pushYBN(&ybnRoot, ybnrem);
            ybn_free(ybn1);
            ybn_free(ybn2);

            if (comparedBNYBN(quo, ybnquo) != 0)
                return -1;
            if (comparedBNYBN(rem, ybnrem) != 0)
                return -1;
        }
        else if (strncmp(buffer + i, "sqr", 3) == 0) {
            i += 3;
            bn_t operand1;
            bn_t result = bn_alloc();
            if (result == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_sqr(result, operand1)) == -1) { return -1; }
            push(&root, result);
            bn_free(operand1);

            //ybn
            ybn_t ybn1;
            ybn_t resultYBN = ybn_alloc();
            if (resultYBN == NULL) { return -1; }
            if ((ybn1 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((status = ybn_sqr(resultYBN, ybn1)) == -1) { return -1; }
            pushYBN(&ybnRoot, resultYBN);
            ybn_free(ybn1);

            if (comparedBNYBN(result, resultYBN) != 0)
                return -1;
        }

        else if(strncmp(buffer + i, "modexp", 6) == 0) {
            i += 6;
            bn_t operand1, operand2, operand3;
            bn_t result = bn_alloc();
            if (result == NULL) { return -1; }
            if ((operand3 = pop(&root)) == NULL) { return -1; }
            if ((operand2 = pop(&root)) == NULL) { return -1; }
            if ((operand1 = pop(&root)) == NULL) { return -1; }
            if ((status = bn_modexp(result, operand1, operand2, operand3)) == -1) { return -1; }
            // push back quot
            push(&root, result);
            bn_free(operand1);
            bn_free(operand2);
            bn_free(operand3);

            //ybn
            ybn_t ybn1, ybn2, ybn3;
            ybn_t resultYBN = ybn_alloc();
            if (resultYBN == NULL) { return -1; }
            if ((ybn3 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((ybn2 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((ybn1 = popYBN(&ybnRoot)) == NULL) { return -1; }
            if ((status = ybn_modexp(resultYBN, ybn1, ybn2, ybn3)) == -1) { return -1; }
            pushYBN(&ybnRoot, resultYBN);
            ybn_free(ybn1);
            ybn_free(ybn2);
            ybn_free(ybn3);

            if (comparedBNYBN(result, resultYBN) != 0)
                return -1;
        }

        else if (strncmp(buffer + i, "dup", 3) == 0) {
            i += 3;
            dup(&root);
            dupYBN(&ybnRoot);
        } else if (strncmp(buffer + i, "pop", 3) == 0) {
            i += 3;
            bn_t temp = pop(&root);
            bn_free(temp);

            ybn_t tempYbn = popYBN(&ybnRoot);
            ybn_free(tempYbn);

        } else if (strncmp(buffer + i, "print", 5) == 0) {
            i += 5;
            printf("=======BN is===========\n");
            printStack(&root);
            printf("=======YBN is===========\n");
            printYBNStack(&ybnRoot);
        } else if (strncmp(buffer + i, "swap", 4) == 0) {
            i += 4;
            swap(&root);
            swapYBN(&ybnRoot);
        } else if (strncmp(buffer + i, "dump", 4) == 0) {
            i += 4;
            printf("=======BN is===========\n");
            dumpCall(&root);
            printf("=======YBN is===========\n");
            dumpYBNCall(&ybnRoot);
        } else if (strncmp(buffer + i, "clear", 5) == 0) {
            clear(&root);
            clearYBN(&ybnRoot);
        } else {
            i++;
        }
    }

    return 0;
}
