/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/07/2018 11:41:01 PM
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
#include <stdint.h>
#include <ctype.h>


extern int32_t example1(int32_t a);
extern uint32_t example2(uint32_t a, uint32_t b);
extern int32_t f1(int32_t a);
extern int32_t f2(uint16_t a, uint16_t b);
extern int32_t f3(int32_t a);
extern int32_t f4(uint32_t a, uint32_t b, int32_t c, int32_t d);


int main() {
    char str[256];
    int32_t a, b;
    uint32_t c, d;


    a = 2;
    b = 2222;
    c = 42;
    d = 7;

//    printf("the return of example %3d is %3d\n", testing, example1(testing));
    printf("the return of f1 %3d is %3d\n", a, f1(a));
    printf("the return of f2 %3d is %3d\n", b, f2(a, b));
    printf("the return of f3 %3d is %3d\n", a, f3(b));
    printf("the return of f4 %3d %3d %3d %3d is %3d\n", c, d, a, b, f4(c, d, a, b));



    return 0;

}

int32_t example1(int32_t a){
    return (a^0xFFFFFFFF) + 1;
}


uint32_t example2(uint32_t a, uint32_t b){
    return (1<<b) & a;
}

int32_t f1(int32_t a){
    return a & -a;
}
int32_t f2(uint16_t a, uint16_t b){
    return ((int32_t)a - (int32_t)b) >> 31;
}
int32_t f3(int32_t a){
    return (a | -a) >> 31;
}
int32_t f4(uint32_t a, uint32_t b, int32_t c, int32_t d){
    c ^= d;
    c = (c | -c) >> 31;
    return (a & ~c) | (b & c);
}
