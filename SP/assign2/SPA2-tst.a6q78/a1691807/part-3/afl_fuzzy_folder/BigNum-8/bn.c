#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"

struct bn {
    int bn_len; //Number of digits represented
    int bn_size; //Num of digits allocated in data
    int bn_sign; //Sign (Always 1)
    uint16_t *bn_data; //Values of each digit
};

//Base 2^16, the size of each digit in a bn_t
static int BASE = 65536;


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


//
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


//Convert bn_t to decimal
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


void bn_setvalues(bn_t bn, int size, uint16_t *bn_data) {
    bn->bn_len = size;
    bn_resize(bn, size);
    for(int i=0 ; i<size ; i++) {
        bn->bn_data[size-i-1] = bn_data[i];
    }
    bn->bn_size = size;
}


//Create a new bn_t
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

//Frees all resources in bn
void bn_free(bn_t bn) {
    free(bn->bn_data);
    free(bn);
}


//Copy a big num
bn_t bn_copy(bn_t in) {
    bn_t out = bn_alloc();
    bn_resize(out, in->bn_size);
    out->bn_size = in->bn_size;
    out->bn_len = in->bn_len;
    for(int i=0 ; i<in->bn_len ; i++) {
        out->bn_data[i] = in->bn_data[i];
    }
    return out;
}


//Display a number in big-endian notation
void bn_print(bn_t bn) {
    printf("\n{");
    for(int i=0 ; i<bn->bn_len ; i++) {
        printf("%d", bn->bn_data[i]);
        if(i != bn->bn_len-1) {
            printf(", ");
        }
    }
    printf("}\n");
}


//Adds so that a+b=result
int bn_add(bn_t result, bn_t a, bn_t b) {

    if((a == NULL) || (b == NULL)) {
        return -1;
    }

    bn_t a_copy = bn_copy(a);
    bn_t b_copy = bn_copy(b);

    int shorter = a_copy->bn_len;
    int longer = b_copy->bn_len;
    int bigger_a = 0;

    if(b_copy->bn_len < a_copy->bn_len) {
        shorter = b_copy->bn_len;
        longer = a_copy->bn_len;
        bigger_a = 1;
    }

    if(bigger_a) {
        bn_resize(result, a_copy->bn_len+1);
    } else {
        bn_resize(result, b_copy->bn_len+1);
    }

    result->bn_len = longer+1;

    uint16_t temp = 0;
    uint16_t carry = 0;
    for(int i=0 ; i<shorter ; i++) {
        temp = a_copy->bn_data[i] + b_copy->bn_data[i] + carry;

        if((temp < a_copy->bn_data[i]) || (temp < b_copy->bn_data[i])) {
            carry = 1;
        }
        else {
            carry = 0;
        }
        result->bn_data[i] = temp;
    }
    for(int i=shorter ; i<longer ; i++) {
        if(bigger_a == 0) {
            temp = b_copy->bn_data[i] + carry;
            if(temp < b_copy->bn_data[i]) {
                carry = 1;
            }
            else {
                carry = 0;
            }
        }
        else {
            temp = a_copy->bn_data[i] + carry;
            if(temp < a_copy->bn_data[i]) {
                carry = 1;
            }
            else {
                carry = 0;
            }
        }
        result->bn_data[i] = temp;
    }

    if(carry == 1) {
        result->bn_data[longer] = carry;
    }

    if((result->bn_data[result->bn_len-1] == 0) && (result->bn_len > 0)) {
        result->bn_len--;
    }

    bn_free(a_copy);
    bn_free(b_copy);

    return 0;
}


//Subtracts so that a-b=result
int bn_sub(bn_t result, bn_t a, bn_t b) {
    if((a == NULL) || (b == NULL)) {
        return -1;
    }

    int shorter = b->bn_len;
    int longer = a->bn_len;
    //If a<b, result=0
    if(a->bn_len < shorter) {
        bn_resize(result, 1);
        result->bn_len = 1;
        result->bn_data[0] = 0;
        return 0;
    }
    else if(a->bn_len == shorter) {
        for(int i=0 ; i<shorter ; i++) {
            if(a->bn_data[shorter-1-i] < b->bn_data[shorter-1-i]) {
                bn_resize(result, 1);
                result->bn_data[0] = 0;
                return 0;
            }
            else if(a->bn_data[shorter-1-i] > b->bn_data[shorter-1-i]) {
                break;
            }
        }

    }

    bn_t a_copy = bn_alloc();
    a_copy->bn_len = a->bn_len;
    a_copy->bn_size = a->bn_size;
    for(int i=0 ; i<a->bn_len ; i++) {
        a_copy->bn_data[i] = a->bn_data[i];
    }

    bn_resize(result, a->bn_size);
    uint16_t temp = 0; 
    for(int i=0 ; i<shorter ; i++) {
        if(a_copy->bn_data[i] >= b->bn_data[i]) {
            temp = a_copy->bn_data[i] - b->bn_data[i];
        }
        else {
            for(int j=i+1 ; j<longer ; j++) {
                if(a_copy->bn_data[j] == 0) {
                    a_copy->bn_data[j] = BASE-1;
                }
                else {
                    a_copy->bn_data[j]--;
                    break;
                }
            }

            temp = BASE - b->bn_data[i];
            temp += a->bn_data[i];
        }
        result->bn_data[i] = temp;
        result->bn_len++;
    }
    for(int i=shorter ; i<longer ; i++) {
        result->bn_data[i] = a_copy->bn_data[i];
        result->bn_len++;
    }

    bn_free(a_copy);
    return 0;
}


//Multiplies so that a*b=result
int bn_mul(bn_t result, bn_t a, bn_t b) {
    if((a == NULL) || (b == NULL)) {
        return -1;
    }
    bn_t bigger;
    bn_t smaller;
    if(b->bn_len > a->bn_len) {
        bigger = b;
        smaller = a;
    }
    else {
        bigger = a;
        smaller = b;
    }
    uint32_t temp = 0;
    uint16_t carry = 0;
    uint16_t remain = 0;
    bn_t steps[smaller->bn_len];

    for(int i=0 ; i<smaller->bn_len ; i++) {
        steps[i] = bn_alloc();
        bn_resize(steps[i], bigger->bn_len + 1 + i);
        for(int k=0 ; k<i ; k++) {
            steps[i]->bn_data[k] = 0;
            steps[i]->bn_len++;
        }
        for(int j=0 ; j<bigger->bn_len ; j++) {
            temp = smaller->bn_data[i] * bigger->bn_data[j];
            remain = (temp + carry) % BASE;
            carry = (temp + carry) / BASE;
            steps[i]->bn_data[j+i] = remain;
            steps[i]->bn_len++;
        }
        if(carry != 0) {
            steps[i]->bn_data[bigger->bn_len + i] = carry;
            steps[i]->bn_len++;
            carry = 0;
        }
    }

    bn_t res_temp = bn_copy(steps[0]);
    for(int i=1 ; i<smaller->bn_len ; i++) {
        bn_add(res_temp, res_temp, steps[i]);
        bn_free(steps[i]);
    }
    
    bn_resize(result, res_temp->bn_size);
    result->bn_len = res_temp->bn_len;
    for(int i=0 ; i<res_temp->bn_len ; i++) {
        result->bn_data[i] = res_temp->bn_data[i];
    }
    bn_free(res_temp);

    return 0;
}


//Check if undergraduate
int bn_IAmAnUndergrad() {
    return 1;
}


//Converts decimal to big num
int bn_fromString(bn_t bn, const char *s) {
    bn_t ten = bn_alloc();
    bn_t c = bn_alloc();
    ten->bn_len = 1;
    c->bn_len = 1;
    ten->bn_data[0] = 10;
    c->bn_data[0] = 0;
    bn->bn_len = 1;
    bn->bn_data[0] = 0;

    for(int i=0 ; s[i] != '\0' ; i++) {
        c->bn_data[0] = s[i]-'0';
        bn_mul(bn, bn, ten);
        bn_add(bn, bn, c);
    }
    return 0;
}


//Converts big num to a string
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
