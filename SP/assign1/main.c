/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/2018 09:52:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:  
 *
 * =====================================================================================
 */

#include "bn.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

int main() {
    char buf[10000], buf1[10000], buf2[1000];
    bn_t a = bn_alloc();
    bn_t b = bn_alloc();
    int statusSub = 0;

    bn_fromString(a, "0");
    bn_fromString(b, "1");
    bn_toString(b, buf, sizeof(buf));
    printf("The b is: %s\n\n", buf);


    int i;

    for (i=0; i<4000; i++) {
        bn_toString(a, buf, sizeof(buf));
        bn_toString(b, buf1, sizeof(buf1));
        printf("%2d: %s, b: %s\n", i, buf,buf1);

        bn_add(a, a, b);
        bn_t t = a;
        a = b;
        b = t;
    }

    bn_toString(a, buf, sizeof(buf));
    bn_toString(b, buf1, sizeof(buf1));
    printf("\n\nBefore Substract: a: %s,   b: %s\n\n", buf, buf1);
    printf("\n\nTry substraction:\n");

    for (i=0; i<4000; i++) {

        statusSub = bn_sub(b, b, a);
        bn_toString(a, buf, sizeof(buf));
        bn_toString(b, buf1, sizeof(buf1));
        printf("status:%d, a: %s, b: %s\n", statusSub, buf, buf1);
        bn_t t = b;
        b = a;
        a = t;
    }

//    bn_t c = bn_alloc();
//    bn_t d = bn_alloc();
//    bn_fromString(c, "9");
//    bn_fromString(d, "9999");
//    bn_toString(c, buf, sizeof(buf));
//    bn_toString(d, buf1, sizeof(buf1));
//    printf("\n\nBefore Multiplication: a: %s,   b: %s\n\n", buf, buf1);
//    printf("\n\nTry Multiplication:\n");
//    for (i=0; i<10; i++) {
//        bn_mul(d, d, c);
//        bn_toString(d, buf, sizeof(buf));
//        printf("%2d: %s\n", i, buf);
//    }


    return 0;
}
