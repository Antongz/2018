#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct bn {
   int bn_len;
   int bn_size;
   int bn_sign;
   uint16_t *bn_data;
};

// // Prints contents of bignum struct (used for debugging)
// static int bn_print(bn_t bn){
//   printf("bn_size: %d\n", bn->bn_size);
//   printf("bn_sign %d\n", bn->bn_sign);
//   printf("bn_len %d\n", bn->bn_len);
//   for (int i=0;i<bn->bn_size;i++){
//     printf("bn_data[%d] %d\n",i, bn->bn_data[i]);
//   }
//   return 0;
// }

void bn_free(bn_t bn){
  free(bn->bn_data);
  free(bn);
}

// Resizes allocation of bn->data to size
static int bn_resize(bn_t bn, int size) {
   // Given size is less or equal to the size of bn
   if (size <= bn->bn_size)
      return 0;
   // Resizes data to size 
   uint16_t *data = (uint16_t *)realloc(bn->bn_data, size * sizeof(uint16_t));
   if (data == NULL)
      return -1;
   // Zeroes out data of new section
   for (int i = bn->bn_size; i < size; i++)
      data[i] = 0;
   // Updates bn data to newly sized data
   bn->bn_data = data;
   // Updates size of bn to new size
   bn->bn_size = size;
   return 1;
}

// Returns real length of a bignum (ignores leading zeros)
static int bn_reallen(bn_t bn) {
   int l = bn->bn_len;
   while (l-- > 0) {
      if (bn->bn_data[l] != 0){
         return l+1;
      }
   }
   return 0;
}

// Decimal bignum push?
static void dbn_push(bn_t bn, uint8_t data) {
   // Carry data
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
   //Binary length
   int binlen = bn_reallen(bn);
   // decimal length
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
   
// Allocates a bn to default value 0
bn_t bn_alloc(void) {
   // Allocate bn
   bn_t bn = (bn_t)malloc(sizeof(struct bn));
   if (bn == NULL)
      return NULL;
   // Allocate bn->data
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

// Returns 1 if character is ASCII numeral
// Otherwise returns 0
static int isNumber(char c){
  // ASCII numeric character 0-9
  if (c >= 48 && c <= 57){
    return 1;  
  }
  return 0;
}

/*  bn      Bignum to be converted to text
    *buf    String buffer 
    buflen  Length of buffer

    return  0 if conversion successful, otherwise
            Number of chars needed to store bn as a string                  
*/
int bn_toString(bn_t bn, char *buf, int buflen) {
   bn_t dbn = todec(bn);

   if (dbn == NULL)
      return -1;

   int dlen = dbn->bn_len;
   uint16_t *data = dbn->bn_data;
   int requiredlen;

   if (dlen == 0){
      requiredlen = 2;
   }
   else{
      requiredlen   = 2 + (bn->bn_sign < 0) + (dlen - 1) * 4 +
   (data[dlen-1] > 999) + 
	(data[dlen-1] > 99) + 
	(data[dlen - 1] > 9);
   }

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
// Changes sign of bn to positive
static void bn_makePositive(bn_t bn){
  bn->bn_sign = 1;
}

// Changes sign of bn to be negative
static void bn_makeNegative(bn_t bn){
  bn->bn_sign = -1;
}

/*
* Chops leading zeros from most significant bit
* of bn to first non-zero digit
*/
static int bn_shrink(bn_t bn){
  int reallen = bn_reallen(bn);

  if (reallen == 0) reallen = 1;
  //printf("IN SHRINK: reallen %d\n", reallen); //TODO DEBUG
  if (reallen > 0){
      uint16_t * data = (uint16_t *) realloc(bn->bn_data, (reallen) * sizeof(uint16_t));
    if (data == NULL){
      return -1;
    }
    bn->bn_data = data;
    bn->bn_len = reallen;
    bn->bn_size = reallen;
  }

  // Special case zero, cannot be negative
  if (bn->bn_size == 1 && bn->bn_data[0] == 0){
    bn->bn_len = 0;
    bn_makePositive(bn);
  }
  return 0;
}


static int bn_grow(bn_t bn, int size){
  
  if (bn->bn_size > size){
    return -1;
  }
  uint16_t * data = (uint16_t *) realloc(bn->bn_data, size * sizeof(uint16_t));
  if (data == NULL){
    return -1;
  }

  bn->bn_data = data;
  bn->bn_size = size;

  return 1;
}

/*
* Append k zero digits to bignum
* (equivalent to multiplying by 2^k)
*/
static int bn_timesBaseK(bn_t result, bn_t a, int k){
  free(result->bn_data);
  uint16_t * data = (uint16_t *)calloc(k + a->bn_size, sizeof(uint16_t));  
  // result->bn_data = (uint16_t *)calloc(k + a->bn_size, sizeof(uint16_t));
  if (data == NULL){
    free(result);
    return -1;
  }

  for (int i = 0; i < a->bn_size; i++){
    data[i+k] = a->bn_data[i];
  }
  result->bn_data = data;
  result->bn_size = a->bn_size + k;
  result->bn_len = a->bn_len + k;
  return 0;
}


/*
* Splts bignum a at position k/2, producing 
* a1 (ceil(k/2) most sig digits) and a0 (k/2 least significant digits)  
* Returns 0 if completed successfully, otherwise -1
*/
static int bn_split(bn_t a1, bn_t a0, bn_t a){
  // Split at point 
  int k2 = (a->bn_size+1)/2; // ceil(k/2)
  int a1Size = k2;
  int a0Size = a->bn_size - k2;

  int a1Resize = bn_resize(a1, a1Size);
  int a0Resize = bn_resize(a0, a0Size);

  if (a1Resize == -1 || a0Resize == -1){
    return -1;
  }
  // Fill a1 with top 
  for (int i=0; i<a->bn_len;i++){
    if (a0Size + i < a->bn_len){
      a1->bn_data[i] = a->bn_data[a0Size+i];
      // printf("SPLIT ADDING TO a1: %d from pos %d \n",a->bn_data[a0Size+i], i);  
    }
    
    if (i < a0Size) {
      a0->bn_data[i] = a->bn_data[i];
    }
  }

  a1->bn_size = a1Size;
  a0->bn_size = a0Size;
  a1->bn_len = MAX(a->bn_len - a0Size, 0);
  a0->bn_len = a0Size;
  bn_shrink(a0);
  bn_shrink(a1);
  return 0;
}

/*
* Adds the bignums a and b and places the sum in result
* Returns 0 if successful, otherwise -1
*/
int bn_add(bn_t result, bn_t a, bn_t b){
  
  // Maximum possible number of digits from adding a and b
  int maxSize = MAX(a->bn_size, b->bn_size) + 1;

  // Attempt to resize result to fit maximum size
  int resize = bn_resize(result, maxSize);
  // Return error if failed
  if (resize == -1){
    return -1;
  }

  long carry = 0;

  for (int i = 0; i < maxSize; i++){
    // For every digit of a
    if (i < a->bn_len){
      // Add if positive
      if (a->bn_sign > 0) {
        carry += a->bn_data[i];
      }
      // Subtract if negative
      else {
        carry -= a->bn_data[i];
      }
    }
    // For every digit of b
    if (i < b->bn_len){
      // Add if positive
      if (b->bn_sign > 0){
        carry += b->bn_data[i];
      }
      // Subtract if negative
      else {
        carry -= b->bn_data[i];
      }
    }
    // Put carry into result
    result->bn_data[i] = (uint16_t) carry;
    // Shift carry for next digit up
    carry >>= 16;
  }
  // If carry ends negative, need to change sign of result
  // and apply borrows to whole
  if (carry < 0){
    carry = 0;
    for (int i = 0; i < maxSize; i++){
      carry -= result->bn_data[i];
      result->bn_data[i] = (uint16_t) carry;
      carry >>= 16;
    }
    bn_makeNegative(result);
  }
  result->bn_len = maxSize;
  bn_shrink(result);
  return 0;
}

/*
* Substracts the bignums b from a and places the sum in result
* Returns 0 if successful, otherwise -1
*/
int bn_sub(bn_t result, bn_t a, bn_t b){
  
  // Maximum possible number of digits from subtracting a and b
  int maxSize = MAX(a->bn_len, b->bn_len) + 1;

  // Attempt to resize result to fit maximum size
  int resize = bn_resize(result, maxSize);
  // Return error if failed
  if (resize == -1){
    return -1;
  }

  long carry = 0;

  for (int i = 0; i < maxSize; i++){
    // For every digit of a
    if (i < a->bn_len){
      // Add if positive
      if (a->bn_sign > 0) {
        carry += a->bn_data[i];
      }
      // Subtract if negative
      else {
        carry -= a->bn_data[i];
      }
    }
    // For every digit of b
    if (i < b->bn_len){
      // Subtract if positive
      if (b->bn_sign > 0){
        carry -= b->bn_data[i];
      }
      // Add if negative
      else {
        carry += b->bn_data[i];
      }
    }
    // Put carry into result
    result->bn_data[i] = (uint16_t) carry;
    // Shift carry for next digit up
    carry >>= 16;
  }
  // If carry ends negative, need to change sign of result
  // and apply borrows to whole
  if (carry < 0){
    carry = 0;

    for (int i = 0; i < maxSize; i++){
      carry -= result->bn_data[i];
      result->bn_data[i] = (uint16_t) carry;
      carry >>= 16;
    }
    bn_makeNegative(result);
  }
  result->bn_len = maxSize;
  bn_shrink(result);
  return 0;
}

/*
* Applies system method of multiplication to a and b
* Places product in result
*/
static int bn_school_mul(bn_t result, bn_t a, bn_t b){
  if (bn_resize(result, 2) == -1){
    // printf("failed to resize");
    return -1;

    result->bn_len = 1;
    result->bn_size = 2;
  }
  // Multipliers too long
  if (a->bn_len > 1 || b->bn_len > 1){
    return -1;  
  }
  long carry = a->bn_data[0] * b->bn_data[0];
  result->bn_data[0] = (uint16_t) carry;
  
  if ((carry >>=16) != 0){
    result->bn_data[1] = carry;
    result->bn_len = 2;
  }
  bn_shrink(result);
  return 0;
}

/*
* Mutliplies bignums a and b and places product in result
* Returns 0 if complete, -1 otherwise
*/
int bn_mul(bn_t result, bn_t a, bn_t b){
  int signA = a->bn_sign;
  int signB = b->bn_sign;

  bn_makePositive(a);
  bn_makePositive(b);
  
  // Deal with zero case instantly
  if (a->bn_len == 0 && a->bn_data[0] == 0)
   {
    bn_resize(result, 1);
    result->bn_data[0] = 0;
    result->bn_len = 0;

    a->bn_sign = signA;
    b->bn_sign = signB;

    return 0;
  }

  if (b->bn_len == 0 && b->bn_data[0] == 0){
    bn_resize(result, 1);
    result->bn_data[0] = 0;
    result->bn_len = 0;

    a->bn_sign = signA;
    b->bn_sign = signB;
    
    return 0;
  }

  // Padding numbers with zeros so they have same number of digits
  if (a->bn_size != b->bn_size){
    int resize = 1;//TODO FIX GROW
    if (a->bn_size < b->bn_size){
      resize = bn_grow(a, b->bn_size);
    }
    else {
      resize = bn_grow(b, a->bn_size);
    }
    if (resize == -1)
      return -1;
  }

  // If digit length is 1, base case multiplication
  if (a->bn_len <= 1 && b->bn_len <= 1){
    if (bn_school_mul(result, a, b)==-1){
      return -1;
    }
  }
  // Karatsuba multiplication
  else{
 
    // Split components of a and b
    bn_t a1 = bn_alloc();
    bn_t a0 = bn_alloc();
    bn_t b1 = bn_alloc();
    bn_t b0 = bn_alloc();

    // Results of component multiplication
    bn_t r0 = bn_alloc();
    bn_t r1 = bn_alloc();
    bn_t r2 = bn_alloc();

    // Split a and b
    bn_split(a1, a0, a);
    bn_split(b1, b0, b);


    bn_t a0SubA1 = bn_alloc();
    bn_t b0SubB1 = bn_alloc();


    bn_sub(a0SubA1, a0, a1);
    bn_sub(b0SubB1, b0, b1);

    int sign = a0SubA1->bn_sign * b0SubB1->bn_sign * -1;// added -1
    
    bn_makePositive(a0SubA1);
    bn_makePositive(b0SubB1);
    
    bn_mul(r2, a0SubA1, b0SubB1);
    bn_free(a0SubA1);
    bn_free(b0SubB1);

    bn_mul(r1, a1, b1);
    bn_free(a1);
    bn_free(b1);
    r2->bn_sign = sign;
    
    bn_t rk = bn_alloc();
    bn_add(rk, r2, r1); // Add R1 and R2

    bn_free(r2);

    bn_mul(r0, a0, b0);
    bn_free(a0);
    bn_free(b0);
    bn_add(rk, rk, r0); // Add R0

    int k = a->bn_size - ((a->bn_size + 1)/2);

    bn_timesBaseK(result, rk, k); // R^K
    bn_free(rk);

    bn_add(result, result, r0);
    bn_free(r0);
    bn_t r2k = bn_alloc();
    bn_timesBaseK(r2k, r1, 2*k); //R1^2K
    bn_free(r1);

    bn_add(result, r2k, result);
    bn_free(r2k);
    
    bn_shrink(result);

  }

  if (signA == signB){
      bn_makePositive(result);
    }
    else{
      bn_makeNegative(result);
    }

    a->bn_sign = signA;
    b->bn_sign = signB;

    return 0;

}


/**
* Converts a string to a bignum
* Returns 0 if successful, otherwise -1
**/
int bn_fromString(bn_t bn, const char *str){
  // Get length of string
  // printf("fromstring: %s\n", str);

  int len = strlen(str);
  // No null strings
  if (len < 1){ //TODO
    return -1;
  }
 
  // Length 1 strings must only contain an integer
  if (len == 1){
    if (isNumber(str[0]) == 1){
      bn->bn_data[0] = str[0]-'0';
      bn->bn_len = 1;
      return 0;
    }
    else{
      return -1;
    }
  }

  int i = 0;
  
  int sign = 1; // Sign is positive by default

  // Negative number 
  if (str[0] == '-'){
    sign = -1;
    i = 1;
  }

  // Positive number
  if (str[0] == '+') {
    sign = 1;
    i = 1;
  }

  // Numbers larger than 1 digit multiplied by 10
  // for each digit
  bn_t ten = bn_alloc();
  ten->bn_len = 1;
  ten->bn_data[0] = 10;

  while (i<len){
    if (isNumber(str[i]) != 1){
      return -1;
    }
    bn_mul(bn, bn, ten);
    bn_t temp = bn_alloc();
    temp->bn_data[0] = str[i] - '0';
    temp->bn_len = 1;
    bn_add(bn, bn, temp);
    bn_free(temp);
    i++;     
  }
  bn->bn_sign = sign;

  bn_free(ten);

  return 0;
}

int bn_IAmAnUndergrad(){
   return 0;
}

