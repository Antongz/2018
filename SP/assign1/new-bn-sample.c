#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

#include "bn.h"

struct bn {
    int bn_len;
    int bn_size;
    int bn_sign;
    uint16_t *bn_data;
};

static int bn_resize(bn_t bn, int size) {
    if (size <= bn->bn_size)
        return 0;
    uint16_t *data = (uint16_t *)realloc(bn->bn_data, size * sizeof(uint16_t));
    if (data == NULL)
        return -1;
    for (int i = bn->bn_size; i < size; i++)
        data[i] = 0;
    bn->bn_data = data;
    bn->bn_size = size;
    return 1;
}


static int bn_reallen(bn_t bn) {
    int l = bn->bn_len;
    while (l-- > 0) {
        if (bn->bn_data[l] != 0)
            return l+1;
    }
    return 0;
}

static void dbn_push(bn_t bn, uint8_t data) {
    uint32_t carry = data;
    for (int j = 0; j < bn->bn_len; j++) {
        carry += bn->bn_data[j] * 256;
        bn->bn_data[j] = carry % 10000;
        carry = carry / 10000;
    }
    if (carry != 0)
        bn->bn_data[bn->bn_len++] = carry;
}

static bn_t todec(bn_t bn) {
    int binlen = bn_reallen(bn);
    int declen = ((binlen + 3)/4) * 5;
    bn_t dbn = bn_alloc();
    if (dbn == NULL)
        return NULL;
    bn_resize(dbn, declen);
    for (int i = binlen; i--; ) {
        dbn_push(dbn, bn->bn_data[i] >> 8);
        dbn_push(dbn, bn->bn_data[i] & 0xFF);
    }
    return dbn;
}


bn_t bn_alloc(void) {
    bn_t bn = (bn_t)malloc(sizeof(struct bn));
    if (bn == NULL)
        return NULL;
    bn->bn_data = (uint16_t *)calloc(1, sizeof(uint16_t));
    if (bn->bn_data == NULL) {
        free(bn);
        return NULL;
    }
    bn->bn_len = 0;
    bn->bn_size = 1;
    bn->bn_sign = 1;
    return bn;
}


int bn_toString(bn_t bn, char *buf, int buflen) {
    bn_t dbn = todec(bn);
    if (dbn == NULL)
        return -1;
    int dlen = dbn->bn_len;
    uint16_t *data = dbn->bn_data;

    int requiredlen;
    if (dlen == 0)
        requiredlen = 2;
    else
        requiredlen  = 2 + (bn->bn_sign < 0) + (dlen - 1) * 4 +
                (data[dlen-1] > 999) +
                (data[dlen-1] > 99) +
                (data[dlen - 1] > 9);
    if (requiredlen > buflen) {
        bn_free(dbn);
        return requiredlen;
    }

    char *p = buf;

    if (dlen == 0) {
        *p++ = '0';
    } else {
        if (bn->bn_sign < 0)
            *p++ = '-';
        dlen--;
        if (data[dlen] > 999)
            *p++ = '0' + (data[dlen] / 1000) % 10;
        if (data[dlen] > 99)
            *p++ = '0' + (data[dlen] / 100) % 10;
        if (data[dlen] > 9)
            *p++ = '0' + (data[dlen] / 10) % 10;
        *p++ = '0' + data[dlen] % 10;
        while (dlen--) {
            *p++ = '0' + (data[dlen] / 1000) % 10;
            *p++ = '0' + (data[dlen] / 100) % 10;
            *p++ = '0' + (data[dlen] / 10) % 10;
            *p++ = '0' + (data[dlen] / 1) % 10;
        }
    }
    *p = '\0';
    bn_free(dbn);
    return 0;
}

void bn_free(bn_t bn) {
    bn->bn_len = 0;
    bn->bn_size = 0;
    free(bn->bn_data);
    free(bn);
}

bool isValidAddition(uint16_t x, uint16_t y, int carry) {
    if ((uint16_t)(x+y+carry)<x)
        return false;
    return true;
}

int copyBN(bn_t destBN, bn_t copyBn) {
    destBN->bn_sign = copyBn->bn_sign;
    destBN->bn_size = copyBn->bn_size;
    destBN->bn_len = copyBn->bn_len;
    destBN->bn_data = copyBn->bn_data;
    return 0;
}

/* Adds the bignumbs a and b and places the sum in result. Returns 0 if completed successfully and
 * -1 otherwise */
int bn_add(bn_t result, bn_t a, bn_t b) {
    int i = 0, carry = 0;
    uint16_t d_a = 0x00, d_b = 0x00;

    bn_t newR = bn_alloc();
    //  printf("a(size): %d, a(length): %d; b(size): %d, b(length): %d\n", a->bn_size, a->bn_len, b->bn_size, b->bn_len);
    int maxLen = MAX(a->bn_len, b->bn_len);
    bn_resize(newR, maxLen+1);
    newR->bn_len = maxLen;

    for (i=0; i<maxLen; i++) {
        if (a->bn_len-1 < i)  d_a = 0x00;
        else    d_a = a->bn_data[i];


        if (b->bn_len-1 < i)  d_b = 0x00;
        else    d_b = b->bn_data[i];

        // check overflow
        if (isValidAddition(d_a, d_b, carry)) {
            newR->bn_data[i] = d_a + d_b + carry;
            carry = 0;
        }

        // overflow happens
        else {
            newR->bn_data[i] = (uint16_t)(d_a - ((MAXUINT16 - d_b) - carry));
            carry = 1;
            if (i == maxLen-1) {
                newR->bn_len++;
                newR->bn_data[i+1] += carry;
            }
        }
    }
    copyBN(result, newR);
    bn_reallen(newR);
    return 0;
}


bool isValidSubstraction(uint16_t x, uint16_t y) {
    if (x < y)
        return false;
    return true;

}



/* Subtracts the bignum b from a and places the difference in result. Returns 0 if completed successfully
 * and -1 otherwise. */
int bn_sub(bn_t result, bn_t a, bn_t b) {
    int i = 0, j = 0, k=0;
    uint16_t d_a = 0x00, d_b = 0x00;

    bn_t newR = bn_alloc();
    bn_resize(newR, a->bn_len);
    newR->bn_len = a->bn_len;


    printf("a(size): %d, a(length): %d; b(size): %d, b(length): %d\n", a->bn_size, a->bn_len, b->bn_size, b->bn_len);

    if (a->bn_len < b->bn_len) {
        result->bn_len = 1;
        result->bn_size = 1;
        result->bn_data[0] = 0;
        return 3;
    }
    // when a->bn_len >= b->bn_len
    else {
        for (i=0; i<a->bn_len; i++) {
            d_a = a->bn_data[i];

            if (b->bn_len-1<i)  d_b = 0x00;
            else    d_b = b->bn_data[i];

            if (d_a < d_b) {
                j = i + 1;
                while (j < a->bn_len) {
                    d_a = a->bn_data[j];

                    if (b->bn_len-1<j)  d_b = 0x00;
                    else    d_b = b->bn_data[j];

                    if (d_a > d_b) {
                        newR->bn_data[i] = (uint16_t) (a->bn_data[i] + (MAXUINT16 - b->bn_data[i]) + 1);
                        newR->bn_data[j] = (uint16_t) ((d_a - d_b) -1);

                        for (k=i+1; k<j; k++) {
                            d_a = a->bn_data[k];

                            if (b->bn_len-1<k)  d_b = 0x00;
                            else    d_b = b->bn_data[k];
                            newR->bn_data[k] = (uint16_t) (d_a + (MAXUINT16 - d_b));
                        }

                        i = j;
                        break;
                    }
                    j++;

                    if (j==a->bn_len) {
                        result->bn_len = 1;
                        result->bn_size = 1;
                        result->bn_data[0] = 0;
                        return -10;
                    }
                }

            }

            else {
                newR->bn_data[i] = (uint16_t) (d_a - d_b);
            }
        }

        bn_reallen(newR);
        copyBN(result, newR);
        return 0;
    }

    bn_reallen(result);
    return -1;
}

/* Multiplies the bignums a and b and places the product in result. Returns 0 if completed successfully
 * and -1 otherwise. */
int bn_mul(bn_t result, bn_t a, bn_t b) {
    int i = 0, j = 0, k=0, maxLen = MAX(a->bn_len, b->bn_len);
    uint16_t d_a = 0x00, d_b = 0x00, carry = 0x00;
    bn_t newR = bn_alloc();
    bn_resize(newR, 2*maxLen+1);
    newR->bn_len = 2*maxLen+1;



    for (i=0; i<a->bn_len; i++) {
        carry = 0x00;
        
        for (j=0; j<b->bn_len; j++) {


        }

    }



    return -1;
}


/* Read the decimal number in the string str and stores its value in bn. Returns 0 if completed successfully
 * and -1 otherwise.
 * Only need to handle non-negative numbers. */
int bn_fromString(bn_t bn, const char *s) {
    int len = strlen(s);
    int itr = 0, i = 0;

    int digit = 0;
    uint32_t data = 0x00;

    for (i=0; i<len; i++) {
        bn->bn_len++;
        digit = s[i] - '0';
        data += (data * 10) + digit;

        if (data>MAXUINT16) {
            bn->bn_data[itr] = (data % MAXUINT16);
            bn_resize(bn,itr+1);

            itr++;
            data = (data / MAXUINT16);
        } else {
            bn->bn_data[itr] = (data % MAXUINT16);
        }
        printf("the data is %" PRIu16 "\n",data);
        printf("the bn_data is %" PRIu16 ", the len: %d, the size: %d\n", bn->bn_data[itr], bn->bn_len, bn->bn_size);
    }

    char buf[1000];
    bn_toString(bn, buf, sizeof(buf));
    printf("bn_toString: %s\n", buf);

    bn_reallen(bn);

    return 0;
}

int bn_IAmAnUndergrad() {
    return 1;
}
