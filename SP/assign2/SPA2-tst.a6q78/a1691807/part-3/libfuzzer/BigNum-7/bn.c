#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>


#include "bn.h"

const int MAX_VAL = 65536;

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

void bn_free(bn_t bn){
  /*Frees all the resources associated with the bignum bn. */

  free(bn->bn_data);
  free(bn);
}

int bn_add(bn_t result, bn_t a, bn_t b){
 /*Adds the bignums a and b and places the sum in result. Returns 0 if completed successfully and −1
otherwise.*/
  int carry = 0;
  uint32_t addingElements = 0;
  int bigLen, smlLen;
  if(a->bn_len == b->bn_len){
    bigLen = smlLen = a->bn_len;
  } else if(a->bn_len > b->bn_len){
    bigLen = a->bn_len;
    smlLen = b->bn_len;
  } else {
    bigLen = b->bn_len;
    smlLen = a->bn_len;
  }
  if(smlLen == bigLen){
    result->bn_len = 0;
    for(int i=0; i<smlLen; i++){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      addingElements = a->bn_data[i] + b->bn_data[i] + carry;  
      if(addingElements >= MAX_VAL){
        carry = 1;
        result->bn_data[i] = addingElements-MAX_VAL;
      } else {
        result->bn_data[i] = addingElements;
        carry = 0;
      }
    }
    if(carry == 1){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      result->bn_data[result->bn_len-1] = 1;  
    }
  } else if(smlLen == a->bn_len){
    result->bn_len = 0;
    for(int i=0; i<smlLen; i++){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      addingElements = a->bn_data[i] + b->bn_data[i] + carry;
      if(addingElements >= MAX_VAL){
        carry = 1;
        result->bn_data[i] = addingElements-MAX_VAL;
      } else {
        result->bn_data[i] = addingElements;
        carry = 0;
      }
    }
    for(int i=smlLen; i<bigLen; i++){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      addingElements = b->bn_data[i] + carry;
      if(addingElements >= MAX_VAL){
        carry = 1;
        result->bn_data[i] = addingElements-MAX_VAL;
      } else {
        result->bn_data[i] = addingElements;
        carry = 0;
      };
    }
    if(carry == 1){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      result->bn_data[result->bn_len-1] = 1;
    }
  } else if(smlLen == b->bn_len){
    result->bn_len = 0;
    for(int i=0; i<smlLen; i++){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      addingElements = a->bn_data[i] + b->bn_data[i] + carry;
      if(addingElements >= MAX_VAL){
        carry = 1;
        result->bn_data[i] = addingElements-MAX_VAL;
      } else {
        result->bn_data[i] = addingElements;
        carry = 0;
      }
    }
    for(int i=smlLen; i<bigLen; i++){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      addingElements = a->bn_data[i] + carry;
      if(addingElements >= MAX_VAL){
        carry = 1;
        result->bn_data[i] = addingElements-MAX_VAL;
      } else {
        result->bn_data[i] = addingElements;
        carry = 0;
      };
    }
    if(carry == 1){
      result->bn_len++;
      bn_resize(result, result->bn_len);
      result->bn_data[result->bn_len-1] = 1;
    }
  }

  return 0;
}

int bn_sub(bn_t result, bn_t a, bn_t b){
 /*Subtracts the bignum b from a and places the difference in result. Returns 0 if completed successfully
and −1 otherwise.
Undergraduate students: if a is smaller than b, the result is set to 0.*/
  int subElements = 0;
  int bigLen, smlLen;
  if(a->bn_len < b->bn_len){
    return 0;
  } else if(a->bn_len == b->bn_len){
    int lengthTest = a->bn_len-1;
    int aIsBigger = 0;
    while((lengthTest >= 0) && (aIsBigger == 0)){
      if(a->bn_data[lengthTest] < b->bn_data[lengthTest]){
        return 0;
      } else if(a->bn_data[lengthTest] > b->bn_data[lengthTest]){
        aIsBigger++;
      } else {
        lengthTest--;
      }
    }
  }
  bigLen = a->bn_len;
  smlLen = b->bn_len;
  result->bn_len = 0;
  for(int i=0; i<smlLen; i++){
    result->bn_len++;
    bn_resize(result, result->bn_len);
    subElements = a->bn_data[i] - b->bn_data[i]; 
    if(subElements < 0){
      if(bigLen >= i){
        a->bn_data[i+1]--;
        subElements += MAX_VAL;
      }
    } 
    result->bn_data[i]= subElements;
  }
  for(int i=smlLen; i<bigLen; i++){
    result->bn_len++;
    bn_resize(result, result->bn_len);
    result->bn_data[i] = a->bn_data[i];
  }
  return 0;
}

int bn_mul(bn_t result, bn_t a, bn_t b){
  /*Multiplies the bignums a and b and places the product in result. Returns 0 if completed successfully and
−1 otherwise.*/
    uint32_t carry;
    uint32_t currentDigit; 
    int i = 0;
    int j;
    if(((void*)&a->bn_data[0] == (void*)&result->bn_data[0]) && ((void*)&b->bn_data[0] == (void*)&result->bn_data[0])){
      printf("Exiting, errno = %d\n", errno);
      return -1;
    } else if ((void*)&a->bn_data[0] == (void*)&result->bn_data[0]){
      printf("Error bn_mul usage:result != a, errno = %d\n", errno);
      return -1;
    } else if ((void*)&b->bn_data[0] == (void*)&result->bn_data[0]){
      printf("Error bn_mul usage:result != b, errno = %d\n", errno);
      return -1;
    } else {
      for(int i=0; i<result->bn_len; i++){
        result->bn_data[i] = 0;
      }
      result->bn_len = 0;
      bn_resize(result,result->bn_len);
    } 
    for(i=0; i<b->bn_len; i++){
      carry = 0; 
      for(j=i; j<a->bn_len + i; j++){
        result->bn_len++;
        bn_resize(result,result->bn_len);
        currentDigit = result->bn_data[j] + (b->bn_data[i] * a->bn_data[j-i] + carry);
        carry = currentDigit/MAX_VAL;
        result->bn_data[j] = currentDigit%MAX_VAL;
      }
      if(carry){
        result->bn_len++;
        bn_resize(result,result->bn_len);
        currentDigit = result->bn_data[j] + carry;
        carry = currentDigit/MAX_VAL;
        result->bn_data[j] = currentDigit%MAX_VAL;
      }
    }
    return 0;
}

int bn_fromString(bn_t bn, const char *str){
  /*Read the decimal number in the string str and stores its value in bn. Returns 0 if completed successfully
and −1 otherwise.
1
Undergraduate students need only handle non-negative numbers. Postgraduate students need to handle
both positive and negative numbers.*/
  int count = 0;
  
  bn_t a = bn_alloc();
  a->bn_len++;

  bn_t multi10 = bn_alloc();
  multi10->bn_data[multi10->bn_len] = 10;
  multi10->bn_len++;

  while(str[count] != '\0'){
    bn_t temp = bn_alloc();
    bn_resize(temp,bn->bn_len);
    for(int i=0; i<bn->bn_len; i++){
      temp->bn_data[i] = bn->bn_data[i];
    } 
    temp->bn_sign = bn->bn_size;
    temp->bn_size = bn->bn_size;
    temp->bn_len = bn->bn_len;

    a->bn_data[0] = str[count] - '0';
    bn_mul(bn,temp,multi10);    
    bn_add(bn,bn,a);
    count++;
    bn_free(temp);
  }
  bn_free(a);
  bn_free(multi10);
  /*int count = 0;
  bn_t temp = bn_alloc();
  bn->bn_data[0] = 1;
  bn->bn_len = 1;
  
    charStore->bn_data[0] = str[count] - '0';
    printBN(bn);
    bn_mul(temp,bn,charStore);
    printBN(temp);
    bn->bn_len = temp->bn_len;
    for(int i=0; i<bn->bn_len; i++){
      bn_resize(bn,i+1);
      bn->bn_data[i] = temp->bn_data[i];
      temp->bn_data[i] = 0;
    }
    printBN(bn);
    bn_resize(temp,1);
    bn_add(bn,bn,charStore);
    count++;
  }*/
  return 0;
}

int bn_IAmAnUndergrad(){
  /*Returns 1 if you’re an undergraduate student, 0 if you’re a postgraduate student. It is highly recommended
not to return the wrong value. Implementations that return 0 will be marked on handling both positive and
negative numbers.. Implementations that return 1 will only be marked on handling non-negative numbers
only, with a penalty of 20% for postgraduate students that use this option.*/
  return 1;
}

int printBN(bn_t bn){
  printf("Printing BN\n");
  for(int i =0; i<bn->bn_len; i++){
    printf("%d: %d\n", i, bn->bn_data[i]);
  }
  printf("\n");
  return 0;
}

