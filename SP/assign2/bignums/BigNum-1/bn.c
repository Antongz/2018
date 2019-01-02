#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
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
    if (bn_resize(destBN, copyBn->bn_size) == -1) return -1;
    memcpy(destBN->bn_data, copyBn->bn_data, sizeof(uint16_t) * copyBn->bn_size);
    return 0;
}

int bn_add(bn_t result, bn_t a, bn_t b) {
    int i = 0;
    uint16_t carry = 0x00;
    uint32_t d_a = 0x00, d_b = 0x00, data = 0x00;

    int maxLen = MAX(a->bn_len, b->bn_len);
    if (bn_resize(result, maxLen+1) == -1 ) return  -1;
    result->bn_len = maxLen;

    for (i=0; i<maxLen; i++) {
        if (i > a->bn_len-1)  d_a = 0x0000;
        else    d_a = (uint32_t) (a->bn_data[i]);
        if (i > b->bn_len-1)  d_b = 0x0000;
        else    d_b = (uint32_t) (b->bn_data[i]);

        data = (uint32_t) (d_a + d_b + carry);
        result->bn_data[i] = (uint16_t) (data & 0xffff);
        carry = (uint16_t) ((data & 0xffff0000) >> 16);
        if (i == maxLen-1) {
            result->bn_len++;
            result->bn_data[i+1] += carry;
        }
    }
    result->bn_len = bn_reallen(result);
    return 0;
}

int bn_sub(bn_t result, bn_t a, bn_t b) {
    int i = 0, borrow = 0;
    uint32_t d_a = 0x00, d_b = 0x00, tmp = 0x00;
    if (bn_resize(result, a->bn_len) == -1)return -1;
    result->bn_len = a->bn_len;

    if (a->bn_len < b->bn_len) {
        bn_free(result);
        result = bn_alloc();
        if (result == NULL) return -1;
        return 0;
    }
    else if (a->bn_len == b->bn_len) {
        for (i=a->bn_len-1; i>=0; i--) {
            if (a->bn_data[i] > b->bn_data[i])
                break;
            else if (a->bn_data[i] < b->bn_data[i]) {
                bn_free(result);
                result = bn_alloc();
                if (result == NULL) return -1;
                return 0;
            }
        }
    }

    for (i=0; i<a->bn_len; i++) {
        d_a = (uint32_t) a->bn_data[i] + (MAXUINT16 + 1);
        if (i < b->bn_len) { d_b = (uint32_t) (b->bn_data[i] + borrow); }
        else { d_b = (uint32_t) borrow; }
        tmp = d_a - d_b;
        result->bn_data[i] = (uint16_t) (tmp & 0xffff);
        borrow = (tmp <= MAXUINT16);
    }


    result->bn_len = bn_reallen(result);
    return 0;
}

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
            dataArray[i+j] = (uint16_t) (0xffff & tempD);
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

int bn_fromString_itr(bn_t bn, bn_t add_bn) {
    bn_t base10 = bn_alloc();
    if (bn_fromString(base10, "10") == -1) return -1;
    if (bn_mul(bn, bn, base10) == -1) return -1;
    if (bn_add(bn, bn, add_bn) == -1) return -1;
    bn_free(base10);
    bn_free(add_bn);
    return 0;
}

int bn_fromString(bn_t bn, const char *s) {

    int len = strlen(s);
    int itr = 0;
    if (s[0] == '-')
        return -1;

    if (len < 19) {
        char *pEnd;
        unsigned long long int temp;
        temp = strtoull(s, &pEnd, 10);
        while (temp != 0x00) {
            bn->bn_len++;
            if (bn_resize(bn, len + 1) == -1) return -1;
            bn->bn_data[itr] = (temp % (MAXUINT16 + 1));
            itr++;
            temp = temp / (MAXUINT16 + 1);
        }
        bn->bn_len = bn_reallen(bn);
    }
    else {
        bn_t temp;
        for (int i=0; i<len; i++) {
            temp = bn_alloc();
            if (s[i] == '0') {
                if (bn_fromString(temp, "0") == -1) return -1;
            } else if (s[i] == '1') {
                if (bn_fromString(temp, "1") == -1) return -1;
            } else if (s[i] == '2') {
                if (bn_fromString(temp, "2") == -1) return -1;
            } else if (s[i] == '3') {
                if (bn_fromString(temp, "3") == -1) return -1;
            } else if (s[i] == '4') {
                if (bn_fromString(temp, "4") == -1) return -1;
            } else if (s[i] == '5') {
                if (bn_fromString(temp, "5") == -1) return -1;
            } else if (s[i] == '6') {
                if (bn_fromString(temp, "6") == -1) return -1;
            } else if (s[i] == '7') {
                if (bn_fromString(temp, "7") == -1) return -1;
            } else if (s[i] == '8') {
                if (bn_fromString(temp, "8") == -1) return -1;
            } else if (s[i] == '9') {
                if (bn_fromString(temp, "9") == -1) return -1;
            }
            if (bn_fromString_itr(bn, temp) == -1) return -1;
        }
    }
    return 0;
}

int bn_IAmAnUndergrad() {
    return 1;
}
