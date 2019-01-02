#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"

struct bn {
  int bn_len;
  int bn_size;
  int bn_sign;
  uint16_t *bn_data;
};

static int bn_resize(bn_t bn, int size);

int bn_fromString(bn_t bn, const char *str){
    //if the string length is less than 5 then the number cannot exceed 65536 therefore
    //a size of one and you only need to modify bn_data[0]
    if (strlen(str) < 5){
      int bnNum = 0;

      for (int i = 0 ; i < strlen(str) ; i++){

          //get current number, multiply previous by 10 and add the new number
          //i.e for 12345
          //1 => 10 => 12 => 120 => 123 => 1230 => 1234 => 12340 => 12345
          int number = str[i] - '0';
          bnNum *= 10;
          bnNum += number;
      }

      bn -> bn_data[0] = bnNum;

      bn -> bn_len = bn -> bn_size;

      return 0;
    }
        
    //a bn for 10 and a bn for the next number
    bn_t multiple = bn_alloc();
    bn_t currentNum = bn_alloc();
    multiple -> bn_data[0] = 10;
 
    for (int i = 0; i < strlen(str); i++){

        currentNum -> bn_data[0] = str[i] - '0';
        
        //increment by 10 for each digit in the string
        if (bn_mul(bn, bn, multiple) == -1) {
          fprintf(stderr, "Error: Multiply Failed\n");
          return -1;
        }

        //add the current number to the bignum
        if (bn_add(bn, bn, currentNum) == -1) {
          fprintf(stderr, "Error: Add Failed\n");
          return -1;
        }

    }

    bn_free(multiple);
    bn_free(currentNum);
    
    bn -> bn_len = bn -> bn_size;
    return 0;

}

int bn_add(bn_t result, bn_t a, bn_t b){
    int max = 0;
    uint32_t number = 0;

    //storing the largest length and setting result to that length
    max = (a -> bn_size > b -> bn_size) ? a -> bn_size : b -> bn_size;
    if (bn_resize(result, max) == -1){
        fprintf(stderr, "Error: Resize Failed\n");
        return -1;
    }
 
    int carry = 0;

    for (int i = 0; i < max; i++){
        number = 0;

        //adding the digits as long as their size has not been exceeded 
        //(otherwise it adds garbage occasionally for some reason)
        if( i <= a -> bn_size - 1 ) {
          number += a -> bn_data[i];
        }

        if( i <= b -> bn_size - 1 ) {
          number += b -> bn_data[i];
        } 

        number += carry;
 
        //if number exceeded the base then find the overflow and make a carry
        if (number >= 65536){
            number -= 65536;
            carry = 1;
        }

        else{
            carry = 0;
        }
 
        result->bn_data[i] = number;
    }
  
    //set final carry and resize if final carry exceeds the result size
    if (carry == 1){
        if (bn_resize(result, (result -> bn_size) + 1) == -1){
            fprintf(stderr, "Error: Resize Failed\n");
            return -1;
        }
 
        result -> bn_data[ ( result -> bn_size ) - 1 ] += 1;
    }
 
    result -> bn_len = result -> bn_size;
 
    return 0;
}

int bn_sub(bn_t result, bn_t a, bn_t b) {
  int max = 0;

  //result = b - a, so we only iterate along b
  max = b -> bn_size;

  //if a's length is smaller than b's, result is 0 so save some time here
  if( a -> bn_len < b -> bn_len ) {
    result -> bn_data[0] = 0;
    result -> bn_size = 1;
    result -> bn_len = 0;
    return 0;
  }

  for(int i = 0 ; i < max ; i++) {

    //if a digit is smaller than b digit, add base and a digit and remove one from the 
    //next a digit
    if(a -> bn_data[i] < b -> bn_data[i] ) {
      uint32_t newVal = a->bn_data[i] + 65536;
      a -> bn_data[i+1]--;
      result -> bn_data[i] = newVal - b -> bn_data[i];
     }

     //else simply do the subtraction
     else {
        result -> bn_data[i] = a -> bn_data[i] - b -> bn_data[i];
     }


  }

  //as resize doesn't resize down, this is another way to do so
  for(int i = result -> bn_size - 1 ; i >= 0 ; i--) {
    if(result -> bn_data[i] != 0) {
      break;
    }

    result -> bn_size--;
  }

  result -> bn_len = result -> bn_size;
  return 0;

}

int bn_mul(bn_t result, bn_t a, bn_t b){
    int curDigit;
    int carry = 0;
    uint32_t number;
    bn_t mulResult = bn_alloc();

    //if one of either a or b is zero then the result will always be zero
    if ( a -> bn_size == 1 && a -> bn_data[0] == 0){
        result->bn_len = 0;
        result->bn_size = 1;
        result->bn_data[0] = 0;
        return 0;
    }

    if( b-> bn_size == 1 && b -> bn_data[0] == 0) {
        result->bn_len = 0;
        result->bn_size = 1;
        result->bn_data[0] = 0;
        return 0;
    }

    for (int i = 0 ; i <= b -> bn_size - 1; i++){
        carry = 0;
        curDigit = i;
       
        for (int j = 0 ; j <= a -> bn_size - 1; j++){
            //resize if current digit exceeds size
            if (curDigit >= mulResult -> bn_size){
                if(bn_resize( mulResult, curDigit + 1 ) == -1){
                    fprintf(stderr, "Error: Resize Failed\n");
                    return -1;
                }
            }
 
            //a value + b value + carry + the result already stored there
            number = ( a -> bn_data[j] * b -> bn_data[i] ) + mulResult -> bn_data[curDigit] + carry;
            mulResult->bn_data[curDigit] = number % 65536;
            carry = number / 65536;
            
            //as per long division, equal to a's digit location + b's digit location
            curDigit = curDigit + 1;
        }
        
        //update size for the extra carry and add it
        if(carry > 0) {
          if(curDigit >= mulResult -> bn_size) {
            if( bn_resize( mulResult, curDigit + 1 )  == -1 ) {
              fprintf(stderr, "Error: Resize Failed\n");
              return -1;
            }
          }

        mulResult->bn_data[curDigit] += carry;

        }

    }

    //set result to the values of mulResult and free previous result data
    result -> bn_size = mulResult -> bn_size;
    free(result -> bn_data);
    result -> bn_data = mulResult -> bn_data;
    result -> bn_len = mulResult -> bn_size;

    return 0;
}

void bn_free(bn_t bn) {
    free(bn -> bn_data);
    free(bn);
}

int bn_IAmAnUndergrad() {
  return 1;
}

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
