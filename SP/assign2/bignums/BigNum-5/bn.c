#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

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

//push data to binary bignum. Same as dbn_push()
static void bn_push(bn_t bn, uint16_t data){
  uint32_t carry = data;
  for(int j = 0; j < bn->bn_len; j++){
    carry += bn->bn_data[j] * 10000;
    bn->bn_data[j] = carry % 65536;
    carry = carry / 65536;
  }
  if(carry != 0)
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

//convert decimal bignum to binary bignum, same as todec()
static bn_t tobin(bn_t dbn) {
  int len = bn_reallen(dbn);
  bn_t bn = bn_alloc();
  if(bn == NULL)
    return NULL;
  if(bn_resize(bn,len) == -1){
    free(bn);
    return NULL;
  }
  for(int i = len; i--; ){
    bn_push(bn, dbn->bn_data[i]);
  }
  return bn;
}

//pow function implement by me.
static int myPow(int base, int exponent){
  int result = 1;
  for(int i=0; i<exponent; i++)
    result *= base;
  return result;
}

//function for copy new bignum to old bignum. And free the old data.
static void bn_copy(bn_t a, bn_t b){
  int tempLen = a->bn_len;
  int tempSize = a->bn_size;
  int tempSign = a->bn_sign;
  uint16_t * tempData = a->bn_data;
  a->bn_len = b->bn_len;
  a->bn_size = b->bn_size;
  a->bn_sign = b->bn_sign;
  a->bn_data = b->bn_data;
  b->bn_len = tempLen;
  b->bn_size = tempSize;
  b->bn_sign = tempSign;
  b->bn_data = tempData;
  bn_free(b);
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

//function for free bignum
void bn_free(bn_t bn) {
  int l = bn->bn_len;
  memset(bn->bn_data, 0, l*sizeof(uint16_t));
  free(bn->bn_data);
  bn->bn_len = 0;
  bn->bn_size = 0;
  bn->bn_sign = 0;
  free(bn);
}

int bn_add(bn_t result, bn_t a, bn_t b){
  int alen = bn_reallen(a);
  int blen = bn_reallen(b);
  int newlen = alen>blen?alen:blen;
  if(newlen < INT_MAX)
    newlen++;
  bn_t newbn = bn_alloc();
  if(newbn == NULL)
    return -1;
  if(bn_resize(newbn,newlen) == -1){
    bn_free(newbn);
    return -1;
  }
  uint32_t carry = 0;
  for (int i = 0; i < newlen; i++) {
    uint16_t adata = i<alen?a->bn_data[i]:0;
    uint16_t bdata = i<blen?b->bn_data[i]:0;
    carry += adata + bdata;
    newbn->bn_data[i] = carry % 65536;
    carry = carry / 65536;
  }
  if(carry == 0){
    bn_copy(result,newbn);
    result->bn_len = result->bn_size;
    result->bn_len = bn_reallen(result);
    return 0;
  }
  else{
    bn_free(newbn);
    return -1;
  }
}

int bn_sub(bn_t result, bn_t a, bn_t b) {
  int alen = bn_reallen(a);
  int blen = bn_reallen(b);
  int newlen = alen>blen?alen:blen;
  bn_t newbn = bn_alloc();
  if(newbn == NULL)
    return -1;
  if(bn_resize(newbn,newlen) == -1){
    free(newbn);
    return -1;
  }
  int32_t carry = 0;
  for (int i = 0; i < newlen; i++) {
    uint16_t adata = i<alen?a->bn_data[i]:0;
    uint16_t bdata = i<blen?b->bn_data[i]:0;
    carry += (signed)adata - (signed)bdata;
    newbn->bn_data[i] = carry<0?65536+carry:carry;
    carry = carry<0?-1:0;
  }
  if(carry!=0) {
    memset(newbn->bn_data, 0, newlen*sizeof(uint16_t));
  }
  bn_copy(result,newbn);
  result->bn_len = result->bn_size;
  result->bn_len = bn_reallen(result);
  return 0;
}

int bn_mul(bn_t result, bn_t a, bn_t b) {
  int alen = bn_reallen(a);
  int blen = bn_reallen(b);
  long newlen = alen+blen;
  if(newlen > INT_MAX)
    newlen = INT_MAX;
  bn_t newbn = bn_alloc();
  if(newbn == NULL)
    return -1;
  if(bn_resize(newbn,(int)newlen) == -1){
    bn_free(newbn);
    return -1;
  }
  bn_t temp = bn_alloc();
  if(temp == NULL){
    bn_free(newbn);
    return -1;
  }
  if(bn_resize(temp,(int)newlen) == -1){
    bn_free(newbn);
    bn_free(temp);
    return -1;
  }
  for(int i=0; i<blen; i++){
    uint32_t carry = 0;
    for(int j=0; j<alen; j++){
      carry += a->bn_data[j] * b->bn_data[i];
      temp->bn_data[i+j] = carry % 65536;
      carry = carry / 65536;
    }
    temp->bn_data[i+alen] = carry;
    temp->bn_len = temp->bn_size;
    temp->bn_len = bn_reallen(temp);
    if(bn_add(newbn, newbn, temp)==-1){
      bn_free(newbn);
      bn_free(temp);
      return -1;
    }
    memset(temp->bn_data, 0, newlen*sizeof(uint16_t));
    temp->bn_len = 0;
  }
  bn_copy(result,newbn);
  result->bn_len = result->bn_size;
  result->bn_len = bn_reallen(result);
  return 0;
}

//Read string to a decimal bignum. Then convert decimal bignum to binary. 
int bn_fromString(bn_t bn, const char *s) {
  int slen = strlen(s);
  int dlen = slen/4+1;
  bn_t dbn = bn_alloc();
  if (dbn == NULL)
    return -1;
  if(bn_resize(dbn,dlen) == -1){
    free(dbn);
    return -1;
  }
  for(int i=0; i<slen; i++){
    int digit = s[i] - '0';
    dbn->bn_data[(slen-i-1)/4] += digit * myPow(10, (slen-i-1)%4);
  }
  dbn->bn_len = dbn->bn_size;
  dbn->bn_len = bn_reallen(dbn);
  bn_t t = bn;
  free(t);
  bn = tobin(dbn);
  if(bn == NULL)
    return -1;
  return 0;
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

int bn_IAmAnUndergrad(void){
  return 1;
}
