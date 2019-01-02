
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"
#include "stack.h"

struct bn 
{
    int bn_len;
    int bn_size;
    int bn_sign;
    uint16_t *bn_data;
};

// allocates space for 1 element in the array
bn_t bn_alloc(void) 
{
    bn_t bn = (bn_t)malloc(sizeof(struct bn));
    if (bn == NULL)
    {
        fprintf(stderr, "Error in bn_alloc: could not allocate memory for data\n");
        return NULL;
    }
        
    bn->bn_data = (uint16_t *)calloc(1, sizeof(uint16_t));
    if (bn->bn_data == NULL) 
    {
        free(bn);
        fprintf(stderr, "Error in bn_alloc: could not allocate memory for data\n");
        return NULL;
    }
    bn->bn_len = 0;
    bn->bn_size = 1;
    bn->bn_sign = 1;
    return bn;
}


void bn_free(bn_t bn)
{
    if(bn != NULL)
    {
        free(bn->bn_data);
        free(bn);
    }  
}


static int bn_resize(bn_t bn, int size) 
{
    if (size <= bn->bn_size)
        return 0;
    uint16_t *data = (uint16_t *)realloc(bn->bn_data, size * sizeof(uint16_t));
    if (data == NULL)
    {
        fprintf(stderr, "Error: could not allocate memory\n");
        return -1;
    }
    for (int i = bn->bn_size; i < size; i++)
        data[i] = 0;
    bn->bn_data = data;
    bn->bn_size = size;
    return 1;
}


static int bn_reallen(bn_t bn) 
{
    int l = bn->bn_len;
    while (l-- > 0) 
    {
        if (bn->bn_data[l] != 0)
            return l+1;
    }
    return 0;
}

static void dbn_push(bn_t bn, uint8_t data) 
{
    uint32_t carry = data;
    for (int j = 0; j < bn->bn_len; j++) 
    {
        carry += bn->bn_data[j] * 256;
        bn->bn_data[j] = carry % 10000;
        carry = carry / 10000;
    }
    if (carry != 0)
        bn->bn_data[bn->bn_len++] = carry;
}

static bn_t todec(bn_t bn) 
{
    int binlen = bn_reallen(bn);
    int declen = ((binlen + 3)/4) * 5;
    bn_t dbn = bn_alloc();
    if (dbn == NULL)
    {
        fprintf(stderr, "Error: could not allocate memory\n");
        return NULL;
    }
    bn_resize(dbn, declen);
    for (int i = binlen; i--; ) {
        // shift 8 bits to right
        dbn_push(dbn, bn->bn_data[i] >> 8);
        // bitwise and with 255
        dbn_push(dbn, bn->bn_data[i] & 0xFF);
    }
    return dbn;
}

static void bn_push(bn_t bn, uint8_t data) 
{
    uint32_t carry = data;
    for (int j = 0; j < bn->bn_len; j++) 
    {
        carry += bn->bn_data[j] * 10;
        bn->bn_data[j] = carry % 65536;
        carry = carry / 65536;
    }
    if (carry != 0)
        bn->bn_data[bn->bn_len++] = carry;
}


int bn_fromString(bn_t bn, const char* s) 
{
    if(bn == NULL)
    {
        fprintf(stderr, "Error in bn_fromString: bn is not declared\n");
        return -1;
    }

    if(bn_resize(bn,(strlen(s)/4)+1)==-1)
    {
        fprintf(stderr, "Error in bn_fromString: could not convert to bignum from string\n");
        return -1;
    }
    for (int i = 0; i<strlen(s); i++) {
        bn_push(bn, s[i]-'0');
    }
    return 0;
}



int bn_add(bn_t result, bn_t a, bn_t b)
{
    if(a == NULL || b == NULL || result == NULL)
    {
        fprintf(stderr, "Error in bn_add: result, a or b is not initialized\n");
        return -1;
    }


    a->bn_len = a->bn_size;
    b->bn_len = b->bn_size;

    a->bn_len = bn_reallen(a);
    b->bn_len = bn_reallen(b);

    int sizea = a->bn_len;
    int sizeb = b->bn_len;

    if(result == NULL)
    {
        fprintf(stderr, "Error in bn_add: bn_t result is not initialized\n");
        return -1;
    }

    uint32_t carry = 0;
    
    if(sizea>=sizeb)
    {
        if(bn_resize(result,sizea+1) == -1)
        {
            fprintf(stderr, "Error in bn_add:\n");
            return -1;
        }
        result->bn_len = sizea+1;
        int i;
        for(i = 0; i<sizeb; i++)
        {
            carry += a->bn_data[i];
            carry += b->bn_data[i];
            result->bn_data[i] = carry % 65536;
            carry = carry / 65536;
        }
        for(i=sizeb; i<sizea; i++)
        {
            carry += a->bn_data[i];
            result->bn_data[i] = carry % 65536;
            carry = carry / 65536;
        }
        result->bn_data[i] = carry;
    }
    else{
        if(bn_resize(result,sizeb+1) == -1)
        {
            fprintf(stderr, "Error in bn_add:\n");
            return -1;
        }
        result->bn_len = sizeb+1;
        int i;
        for(i = 0; i<sizea; i++)
        {
            carry += a->bn_data[i];
            carry += b->bn_data[i];
            result->bn_data[i] = carry % 65536;
            carry = carry / 65536;
        }
        for(i = sizea; i<sizeb; i++)
        {
            carry += b->bn_data[i];
            result->bn_data[i] = carry % 65536;
            carry = carry / 65536;
        }
        result->bn_data[i] = carry;
    }
    result->bn_len = bn_reallen(result);
    return 0;
} 

// returns 1 if b is greater than a
static int bn_compare(bn_t a, bn_t b)
{
    if(a->bn_len < b->bn_len)
    {
        return 1;
    }
    if(a->bn_len == b->bn_len)
    {
        for(int i=a->bn_len-1; i>=0; i--)
        {   
            // for first digit in b not the same as a and bigger then b is greater
            if(a->bn_data[i]!=b->bn_data[i]){
                if(b->bn_data[i]>a->bn_data[i])
                {
                    return 1;
                }
                else{
                    return 0;
                }
            }
        }
    }
    return 0;
}


int bn_sub(bn_t result, bn_t a, bn_t b)
{

    if(a == NULL || b == NULL || result == NULL)
    {
        fprintf(stderr, "Error in bn_sub: result, a or b is not initialized\n");
        return -1;
    }

    a->bn_len = a->bn_size;
    b->bn_len = b->bn_size;

    a->bn_len = bn_reallen(a);
    b->bn_len = bn_reallen(b);

    int sizea = a->bn_len;

    // if a is not greater than b result should be set to 0
    if(bn_compare(a,b) != 0)
    {
        for(int i = 0; i<result->bn_size ; i++)
        {
            result->bn_data[i] = 0;
        }
    }
    else
    {
        if(bn_resize(result, sizea) == -1)
        {
            fprintf(stderr, "Error in bn_sub:\n");
            return -1;
        }
        result->bn_len = sizea;
        for(int i = 0; i<sizea; i++)
        {
            if(a->bn_data[i]<b->bn_data[i])
            {
                a->bn_data[i+1]--;
                result->bn_data[i] = (a->bn_data[i]+65536) - (b->bn_data[i]);
            }
            else
            {
                result->bn_data[i] = a->bn_data[i] - b->bn_data[i];
            }
        }
    }
    result->bn_len = bn_reallen(result);
    return 0;
}


int bn_mul(bn_t result, bn_t a, bn_t b)
{
    if(a == NULL || b == NULL || result == NULL)
    {
        fprintf(stderr, "Error in bn_mul: result, a or b is not initialized\n");
        return -1;
    }

    if(result==a || result==b)
    {
        fprintf(stderr, "Error in bn_mul: bn_t result cannot be the same as bn_t a or bn_t b\n");
        return -1;
    }

    a->bn_len = a->bn_size;
    b->bn_len = b->bn_size;

    a->bn_len = bn_reallen(a);
    b->bn_len = bn_reallen(b);

    int sizea = a->bn_len;
    int sizeb = b->bn_len;

    if(bn_resize(result, sizea+sizeb) == -1)
    {
        fprintf(stderr, "Error in bn_sub:\n");
        return -1;
    }
    result->bn_len = result->bn_size;

    uint32_t carry = 0;
    long temp = 0;

    for(int i = 0; i<sizeb; i++)
    {
        carry = 0;
        for(int j = 0; j<sizea; j++)
        {
            temp = result->bn_data[i+j]+carry+((long)a->bn_data[j]*b->bn_data[i]);
            carry = temp/65536;
            result->bn_data[i+j] = temp % 65536;
        }
        result->bn_data[i+sizea] += carry;
    }

    result->bn_len = bn_reallen(result);
    return 0;
}




int bn_toString(bn_t bn, char *buf, int buflen) 
{
    bn_t dbn = todec(bn);
    if (dbn == NULL)
    {
        fprintf(stderr, "Error in bn_toString: could not allocate memory for data\n");
        return -1;
    }

    if(buf == NULL)
    {
        fprintf(stderr, "Error in bn_toString: buf is not initialized\n");
        return -1;
    }
    int dlen = dbn->bn_len;
    //printf("dbnlen: %i\n",dlen);
    uint16_t *data = dbn->bn_data;

    int requiredlen;
    if (dlen == 0)
        requiredlen = 2;
    else
        requiredlen  = 2 + (bn->bn_sign < 0) + (dlen - 1) * 4 +
    (data[dlen-1] > 999) + 
    (data[dlen-1] > 99) + 
    (data[dlen - 1] > 9);
    if (requiredlen > buflen) 
    {
        bn_free(dbn);
        fprintf(stderr, "Error in bn_toString: required length of buffer is: %i\n",requiredlen);
        return requiredlen;
    }

    char *p = buf;

    if (dlen == 0) 
    {
        *p++ = '0';
    } 
    else 
    {
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
        while (dlen--) 
        {
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

int bn_IAmAnUndergrad()
{
    return 1;
}
