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
int main(int argc, char **argv){
    /*if(argc != 1){
        printf("usage: calc [testfile], errno = %d\n", errno);
        return 1;
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf ("Error, File not opened correctly, errno = %d\n", errno);
        return 1;
    }
    */
    char c, testNum;
    bn_t stack[100];
    int sp = 0;

    while(1) {
        c = fgetc(stdin);
        while(isspace(c)){
            c = fgetc(stdin);
            if( feof(stdin) ) {
                break ;
            }
        }
        if( feof(stdin) ) {
           break ;
        }
        int size = 50;
        int len = 0;
        char *str = (char*)malloc(size * sizeof(char));
        while(!isspace(c)){
            str[len]=c;
            len++;
            if(len==size){
                str = realloc(str, sizeof(char)*(size+=50));
                if(!str){
                    printf("Failed to realloc, errno = %d\n", errno);
                    exit(1);
                }
            }
            c = fgetc(stdin);
            if( feof(stdin) ) {
                break ;
            }
        }
        str[len]='\0';
        if((strcmp(str,"+") == 0) || (strcmp(str,"-") == 0) || (strcmp(str,"*") == 0)){
            action(stack, &sp, str[0]);
        } else if(strcmp("dup",str) == 0){
            dup(stack,&sp);
        } else if(strcmp("pop",str) == 0){
            pop(stack,&sp);
        } else if(strcmp("print",str) == 0){
            print(stack,&sp);
        } else if(strcmp("swap",str) == 0){
            swap(stack,&sp);
        } else if(strcmp("dump",str) == 0){
            dump(stack,sp);
        } else if(strcmp("clear",str) == 0){
            clear(stack,&sp);
        } else {
            int testNumCount = 0;
            testNum = str[testNumCount];
            while(testNum){
                if(isdigit(testNum)){
                    testNumCount++;
                    testNum = str[testNumCount];
                } else {
                    printf("Error, Enter a correct command or number, errno = %d\n", errno);
                    exit(1);
                }
            }
            decimalNumber(stack,&sp,str);
        }
       free(str);
    }
    return 0;
}
