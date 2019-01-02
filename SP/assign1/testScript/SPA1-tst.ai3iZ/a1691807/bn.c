/*
 * =====================================================================================
 *
 *       Filename:  bn-sample.c
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
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <wchar.h>

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
    memset(bn->bn_data, 0, bn->bn_size);
    free(bn->bn_data);
    bn->bn_data = NULL;
    bn->bn_size = 0;
    free(bn);
    bn = NULL;
}

int copyBN(bn_t destBN, bn_t copyBn) {
    destBN->bn_sign = copyBn->bn_sign;
    destBN->bn_len = copyBn->bn_len;
    if (bn_resize(destBN, copyBn->bn_size) == -1){
        return -1;
    }
    memcpy(destBN->bn_data, copyBn->bn_data, sizeof(uint16_t)*copyBn->bn_size);
    return 0;
}

/* Adds the bignumbs a and b and places the sum in result. Returns 0 if completed successfully and
 * -1 otherwise */
int bn_add(bn_t result, bn_t a, bn_t b) {
    int i = 0;
    uint16_t carry = 0x00;
    uint32_t d_a = 0x0000, d_b = 0x0000, data = 0x0000;

    int maxLen = MAX(a->bn_len, b->bn_len);

    if (bn_resize(result, maxLen+1) == -1 ) return  -1;
    result->bn_len = maxLen;

    for (i=0; i<maxLen; i++) {
        if (i > a->bn_len-1)  d_a = 0x0000;
        else    d_a = (uint32_t) (a->bn_data[i]);

        if (i > b->bn_len-1)  d_b = 0x0000;
        else    d_b = (uint32_t) (b->bn_data[i]);

        data = (uint32_t) (d_a + d_b + carry);
        result->bn_data[i] = (uint16_t) (data & 0x0000ffff);
        carry = (uint16_t) ((data & 0xffff0000) >> 16);

        if (i == maxLen-1) {
            result->bn_len++;
            result->bn_data[i+1] += carry;
        }
    }

    result->bn_len = bn_reallen(result);
    return 0;
}

/* Subtracts the bignum b from a and places the difference in result. Returns 0 if completed successfully
 * and -1 otherwise. */
int bn_sub(bn_t result, bn_t a, bn_t b) {
    int i = 0, borrow = 0;
    uint32_t d_a = 0x0000, d_b = 0x0000, tmp = 0x0000;

    a->bn_len = bn_reallen(a);
    b->bn_len = bn_reallen(b);

    if (bn_resize(result, a->bn_len) == -1){
        return -1;
    }
    result->bn_len = a->bn_len;

    if (a->bn_len < b->bn_len) {
        bn_free(result);
        result = bn_alloc();
        if (result == NULL) return -1;

        return 0;
    }
    // when a->bn_len >= b->bn_len
    else if ((a->bn_len == b->bn_len) && (a->bn_data[a->bn_len-1] < b->bn_data[b->bn_len-1])) {
        bn_free(result);
        result = bn_alloc();
        if (result == NULL) return -1;

        return 0;
    }
    else {
        for (i=0; i<a->bn_len; i++) {
            d_a = (uint32_t) a->bn_data[i] + (MAXUINT16 + 1);

            if (i < b->bn_len) { d_b = (uint32_t) (b->bn_data[i] + borrow); }
            else { d_b = (uint32_t) borrow; }

            tmp = d_a - d_b;
            result->bn_data[i] = (uint16_t) (tmp & 0xffff);
            borrow = (tmp <= MAXUINT16);
        }
    }

    result->bn_len = bn_reallen(result);
    return 0;
}

/* Multiplies the bignums a and b and places the product in result. Returns 0 if completed successfully
 * and -1 otherwise. */
int bn_mul(bn_t result, bn_t a, bn_t b) {
    int i = 0, j = 0;
    uint16_t carry = 0x00;
    uint32_t tempD = 0x00;

    if (bn_resize(result, a->bn_len + b->bn_len + 1) == -1) return -1;

    result->bn_len = a->bn_len + b->bn_len + 1;

    uint16_t *dataArray = (uint16_t *) calloc (result->bn_len, sizeof(uint16_t));

    for (i=0; i<a->bn_len; i++) {
        carry = 0x00;

        for (j=0; j<b->bn_len; j++) {
            tempD = (uint32_t) (a->bn_data[i] * b->bn_data[j]);
            tempD = tempD + dataArray[i+j] + carry;
            dataArray[i+j] = (uint16_t) (0x0000ffff & tempD);
            carry = (uint16_t) ((0xffff0000 & tempD) >> 16);

            if (j == b->bn_len-1 && carry != 0)
                dataArray[i+j+1] += carry;
        }
    }

    memset(result->bn_data, 0, result->bn_size);
    free(result->bn_data);
    result->bn_data = dataArray;
    result->bn_len = bn_reallen(result);

    return 0;
}

/* Read the decimal number in the string str and stores its value in bn. Returns 0 if completed successfully
 * and -1 otherwise.
 * Only need to handle non-negative numbers. */
int bn_fromString(bn_t bn, const char *s) {
    int len = strlen(s);
    int itr = 0;

    if (s[0] == '-')
        return -1;

    char *pEnd;
    char buf[10000];

    if (len < 19) {
        unsigned long long int temp;
        temp = strtoull(s, &pEnd, 10);

        while (temp != 0x00) {
            bn->bn_len++;

            if (bn_resize(bn, len + 1) == -1){
                return -1;
            }
            bn->bn_data[itr] = (temp % (MAXUINT16 + 1));

            itr++;
            temp = temp / (MAXUINT16 + 1);
        }
        bn->bn_len = bn_reallen(bn);
    }

    else {
        bn_t a = bn_alloc();
        bn_t b = bn_alloc();
        bn_t base2 = bn_alloc();
        if (a == NULL || b == NULL || base2 == NULL) return -1;


        bn_fromString(a, "1");
        bn_fromString(base2, "2");
        bn_fromString(b, "1");

        int itrTemp = 0;
        if (bn_toString(a, buf, sizeof (buf)) == -1) return -1;


        while ((strlen(s) > strlen(buf)) || ((strlen(s) == strlen(buf)) && strcmp(s, buf) > 0)) {
            memset (buf, 0, 10000);
            bn_mul(a, a, base2);
            if (bn_toString(a, buf, sizeof (buf)) == -1) return -1;
            itr++;
        }

        bn_t c = bn_alloc();
        bn_t tempBN = bn_alloc();
        if (c == NULL || tempBN == NULL) return -1;

        while (itr >= 0) {
            if (strcmp(s, buf) == 0)
                break;

            tempBN = bn_alloc();
            c = bn_alloc();
            if (c == NULL || tempBN == NULL) return -1;

            if (copyBN(tempBN, a) == -1) return -1;
            if (copyBN(c, b) == -1) return -1;

            for (itrTemp=0; itrTemp<itr; itrTemp++)
                if (bn_mul(c, c, base2) == -1) return -1;

            if (bn_sub(tempBN, tempBN, c) == -1) return -1;
            memset (buf, 0, 10000);

            if (bn_toString(tempBN, buf, sizeof (buf)) == -1) return -1;
            if (((strcmp(s, buf) <= 0) && strlen(s) == strlen(buf)) || (strlen(s) < strlen(buf)))
                if (copyBN(a, tempBN) == -1) return -1;


            bn_free(c);
            bn_free(tempBN);
            if (bn_toString(a, buf, sizeof (buf)) == -1) return -1;
            itr--;
        }

        if (strcmp(s, buf) < 0) {
            if (bn_sub(a, a, b) == -1) return -1;
        } else if (strcmp(s, buf) > 0) {
            if (bn_add(a, a, b) == -1) return -1;
        }

        a->bn_len = bn_reallen(a);
        if (copyBN(bn, a) == -1) return -1;
        bn_free(a);
        bn_free(b);
        bn_free(base2);

//        bn_toString(bn, buf, sizeof(buf));
////        printf("%s\n", buf);
    }
    return 0;
}


int bn_IAmAnUndergrad() {
    return 1;
}
