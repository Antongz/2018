#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <ctype.h>
#include "bn.h"
#include "bn.c"

void push(bn_t *stack, int * sp, bn_t toPush){
    stack[*sp] = toPush;
    (*sp)++;
}

void decimalNumber(bn_t *stack, int *sp, char * str){
    bn_t temp = bn_alloc();
    bn_fromString(temp,str);
    push(stack,sp,temp);
}

bn_t pop(bn_t *stack, int *sp){
    (*sp)--;
    return stack[*sp];
}


void action(bn_t *stack, int *sp, char sign){
    switch(sign)
    {
        case '+':
        {
            bn_t res = bn_alloc();
            bn_t b = pop(stack,sp);
            bn_t a = pop(stack,sp);
            bn_add(res,a,b);
            push(stack,sp,res);
            break;
        }
        case '-':
        {
            bn_t res = bn_alloc();
            bn_t b = pop(stack,sp);
            bn_t a = pop(stack,sp);
            bn_sub(res,a,b);
            push(stack,sp,res);
            break;
        }
        case '*':
        {
            bn_t res = bn_alloc();
            bn_t b = pop(stack,sp);
            bn_t a = pop(stack,sp);
            bn_mul(res,a,b);
            push(stack,sp,res);
            break;
        }
    }

}

void dup(bn_t *stack, int *sp){
    bn_t a = pop(stack,sp);
    bn_t b = a;
    push(stack,sp,a);
    push(stack,sp,b);
}

void print(bn_t *stack, int *sp){
    char test[100];
    int buffsize = bn_toString(stack[*sp-1], test, sizeof(test));
    if(buffsize == 0){
        printf("%s\n", test);  
    } else {
        char buf[buffsize];
        bn_toString(stack[*sp-1], buf, sizeof(buf));
        printf("%s\n", buf);
    }
    
}

void swap(bn_t *stack, int *sp){
    bn_t a = pop(stack,sp);
    bn_t b = pop(stack,sp);
    push(stack,sp,b);
    push(stack,sp,a);
}

void dump(bn_t *stack, int sp){
    while(sp >= 0){
        sp--;
        char test[100];
        int buffsize = bn_toString(stack[sp], test, sizeof(test));
        if(buffsize == 0){
            printf("%s\n", test);  
        } else {
            char buf[buffsize];
            bn_toString(stack[sp], buf, sizeof(buf));
            printf("%s\n", buf);
        }
    }
}

void clear(bn_t *stack, int *sp){
    while(*sp > 0){
        pop(stack,sp);
    }
}

char *inputString(FILE* fp){
    char *str;
    int ch;
    size_t size = 50;
    size_t len = 0;
    str = realloc(NULL, sizeof(char)*size);//size is start size
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=50));
            if(!str)return str;
        }
    }
    str[len++]='\0';
    return realloc(str, sizeof(char)*len);
}

//int main(int argc, char **argv){
//    /*if(argc != 1){
//        printf("usage: calc [testfile], errno = %d\n", errno);
//        return 1;
//    }
//    FILE *fp;
//    fp = fopen(argv[1], "r");
//    if (fp == NULL) {
//        printf ("Error, File not opened correctly, errno = %d\n", errno);
//        return 1;
//    }
//    */
//    char c, testNum;
//    bn_t stack[100];
//    int sp = 0;
    
//    while(1) {
//        c = fgetc(stdin);
//        while(isspace(c)){
//            c = fgetc(stdin);
//            if( feof(stdin) ) {
//                break ;
//            }
//        }
//        if( feof(stdin) ) {
//           break ;
//        }
//        int size = 50;
//        int len = 0;
//        char *str = (char*)malloc(size * sizeof(char));
//        while(!isspace(c)){
//            str[len]=c;
//            len++;
//            if(len==size){
//                str = realloc(str, sizeof(char)*(size+=50));
//                if(!str){
//                    printf("Failed to realloc, errno = %d\n", errno);
//                    exit(1);
//                }
//            }
//            c = fgetc(stdin);
//            if( feof(stdin) ) {
//                break ;
//            }
//        }
//        str[len]='\0';
//        if((strcmp(str,"+") == 0) || (strcmp(str,"-") == 0) || (strcmp(str,"*") == 0)){
//            action(stack, &sp, str[0]);
//        } else if(strcmp("dup",str) == 0){
//            dup(stack,&sp);
//        } else if(strcmp("pop",str) == 0){
//            pop(stack,&sp);
//        } else if(strcmp("print",str) == 0){
//            print(stack,&sp);
//        } else if(strcmp("swap",str) == 0){
//            swap(stack,&sp);
//        } else if(strcmp("dump",str) == 0){
//            dump(stack,sp);
//        } else if(strcmp("clear",str) == 0){
//            clear(stack,&sp);
//        } else {
//            int testNumCount = 0;
//            testNum = str[testNumCount];
//            while(testNum){
//                if(isdigit(testNum)){
//                    testNumCount++;
//                    testNum = str[testNumCount];
//                } else {
//                    printf("Error, Enter a correct command or number, errno = %d\n", errno);
//                    exit(1);
//                }
//            }
//            decimalNumber(stack,&sp,str);
//        }
//       free(str);
//    }
//    return 0;
//}
