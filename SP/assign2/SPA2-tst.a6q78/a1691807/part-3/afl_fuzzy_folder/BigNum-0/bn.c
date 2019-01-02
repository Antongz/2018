#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"

/**
 * Frees up the memory taken by bn.
 *
 * @param bn: a bn_t number.
 */
void bn_free(bn_t bn) {
  free(bn->bn_data);
  free(bn);
}


/**
 * Makes a given bn zero by clearing the data.
 *
 * @param num: bn_t to be cleared.
 */
void bn_make_zero(bn_t num) {
  int i;
  for (i = 0; i < num->bn_size; i++) {
    num->bn_data[i] = 0;
  }
  num->bn_len = 0;
}


/**
 * Copies the content a to b. Shallow copy.
 * Frees the data memory of the destination.
 *
 * @param src: source.
 * @param dst: destination.
 */
void bn_copy_shallow(bn_t src, bn_t dst) {
  free(dst->bn_data);
  dst->bn_len = src->bn_len;
  dst->bn_size = src->bn_size;
  dst->bn_sign = src->bn_sign;
  dst->bn_data = src->bn_data;
}

/**
 * Copies the content a to b. Deep copy.
 *
 * @param src: source.
 * @param dst: destination.
 */
void bn_copy_deep(bn_t src, bn_t dst) {
  bn_resize(dst, src->bn_size);
  dst->bn_len = src->bn_len;
  dst->bn_size = src->bn_size;
  dst->bn_sign = src->bn_sign;
  int i;
  for (i = 0; i < src->bn_size; i++) {
    dst->bn_data[i] = src->bn_data[i];
  }
}


/**
 * Creates a 1 digit bn.
 *
 * @params num: uint16 number.
 */
bn_t bn_create_int(uint16_t num) {
  bn_t new_bn = bn_alloc();
  bn_resize(new_bn, 1);
  new_bn->bn_len = 1;
  new_bn->bn_data[0] = num;
  return new_bn;
}


/**
 * Resizes a bn_t to a given size.
 *
 * @param bn: number to resize.
 * @param size: the new size.
 * @return 1 if successful, -1 if unsuccessful.
 */
int bn_resize(bn_t bn, int size) {
  if (size <= bn->bn_size)
    return 0;
  uint16_t *data = (uint16_t *)realloc(bn->bn_data, size * sizeof(uint16_t));
  if (data == NULL)
    return -1;
  int i;
  for (i = bn->bn_size; i < size; i++)
    data[i] = 0;
  bn->bn_data = data;
  bn->bn_size = size;
  return 1;
}


/**
 * Finds the actual length of a bn_t by ignoring the leading 0s.
 * 
 * @param bn: bn_t to be inspected.
 * @return the acutal size of the bn_t.
 */
static int bn_reallen(bn_t bn) {
  int l = bn->bn_len;
  while (l-- > 0) {
    if (bn->bn_data[l] != 0)
      return l+1;
  }
  return 0;
}


/**
 * Pushes a number to decimal bn.
 *
 * @param bn: bn_t to be modified.
 * @param data: number to be added to bn_t.
 */
static void dbn_push(bn_t bn, uint8_t data) {
  uint32_t carry = data;
  int j;
  for (j = 0; j < bn->bn_len; j++) {
    carry += bn->bn_data[j] * 256;
    bn->bn_data[j] = carry % 10000;
    carry = carry / 10000;
  }
  if (carry != 0)
    bn->bn_data[bn->bn_len++] = carry;
}


/**
 * Converts a bn_t to a decimal bn_t. i.e. each digit is based 10000.
 * 
 * @param bn: bn_t to be converted.
 * @return new decimal bn_t.
 */
static bn_t todec(bn_t bn) {
  int binlen = bn_reallen(bn);
  int declen = ((binlen + 3)/4) * 5;
  bn_t dbn = bn_alloc();
  if (dbn == NULL)
    return NULL;
  bn_resize(dbn, declen);
  int i;
  for (i = binlen; i--; ) {
    dbn_push(dbn, bn->bn_data[i] >> 8);
    dbn_push(dbn, bn->bn_data[i] & 0xFF);
  }
  return dbn;
}


/**
 * Allocates memory for a 0 bn.
 *
 * @return pointer to the new bn.
 */
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


/**
 * Turns a bn_t to string. Decimal representation.
 *
 * @param bn: bn_t to be converted.
 * @param buf: empty char array to put the decimal representation into.
 * @param buflen: the length of allocated char array.
 * @return 0 if successful, -1 otherwise.
 */
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


/**
 * Compares 2 big numbers. Returns 1 if a is bigger than b, 0 if they are equal, and -1 otherwise.
 *
 * @param a: bn number 1.
 * @param b: bn number 2.
 */
int bn_compare(bn_t a, bn_t b) {
  int len_num1 = bn_reallen(a);
  int len_num2 = bn_reallen(b);
  if (len_num1 > len_num2) return 1;
  if (len_num1 < len_num2) return -1;
  int i;
  for (i = len_num1-1; i >= 0; i--) {
    if (a->bn_data[i] > b->bn_data[i]) return 1;
    if (a->bn_data[i] < b->bn_data[i]) return -1;
  }
  return 0;
}


/**
 * Adds a and b, stores the results into `result`.
 *
 * @param result: bn_t to store the result.
 * @param a: the first number.
 * @param b: the second number to add.
 * @return 0 if successful, -1 otherwise.
 */
int bn_add(bn_t result, bn_t a, bn_t b) {
  int len_num1 = bn_reallen(a);
  int len_num2 = bn_reallen(b);
  bn_t num1 = a;
  bn_t num2 = b;
  // Num1 is the longer number.
  if (len_num1 < len_num2) {
    num1 = b;
    num2 = a;
    len_num1 ^= len_num2;
    len_num2 ^= len_num1;
    len_num1 ^= len_num2;
  }
  // Make sure result is long enough.
  if (result->bn_size <= len_num1) {
    bn_resize(result, len_num1+1);
  }
  result->bn_len = len_num1;

  uint32_t carry = 0;
  int i;
  for (i = 0; i < len_num1; i++) {
    if (i < len_num2) {
      carry += num1->bn_data[i] + num2->bn_data[i];
    } else {
      carry += num1->bn_data[i];
    }
    result->bn_data[i] = carry % 0x10000;
    carry = carry / 0x10000;
  }
  if (carry != 0) {
    result->bn_data[len_num1] = carry;
    result->bn_len++;
  }
  return 0;
}


/**
 * Subtracts b from a, stores the results into `result`. result = 0 if b > a.
 *
 * @param result: bn_t to store the result.
 * @param a: the first number.
 * @param b: the second number.
 * @return 0 if successful, -1 otherwise.
 */
int bn_sub(bn_t result, bn_t a, bn_t b) {
  int len_num1 = bn_reallen(a);
  int len_num2 = bn_reallen(b);
  int len_re = bn_reallen(result);
  bn_t num1 = a;
  bn_t num2 = b;
  int cmp = bn_compare(a, b);

  if (cmp == 0 || cmp == -1) {
    bn_make_zero(result);
    return 0;
  }

  // Make sure result is long enough.
  if (len_re <= len_num1) {
    bn_resize(result, len_num1);
  }

  int32_t carry = 0;
  int i;
  for (i = 0; i < len_num1; i++) {
    if (i < len_num2) {
      carry += num1->bn_data[i] - num2->bn_data[i];
    } else {
      carry += num1->bn_data[i];
    }
    if (carry > 0) {
      result->bn_data[i] = carry % 0x10000;
      carry = 0;
    } else {
      result->bn_data[i] = (carry + 0x10000) % 0x10000;
      carry = -1;
    }
  }
  if (carry != 0) {
    result->bn_data[len_num1] = carry;
  }
  result->bn_len = len_num1;
  result->bn_len = bn_reallen(result);
  return 0;
}


/**
 * Multiplies a by b, stores the results into `result`.
 *
 * @param result: bn_t to store the result.
 * @param a: the first number.
 * @param b: the second number.
 * @return 0 if successful, -1 otherwise.
 */
int bn_mul(bn_t re, bn_t a, bn_t b) {
  bn_t result = bn_alloc();
  int len_num1 = bn_reallen(a);
  int len_num2 = bn_reallen(b);
  bn_t num1 = a;
  bn_t num2 = b;

  // Num1 is the longer number.
  if (len_num1 < len_num2) {
    num1 = b;
    num2 = a;
    len_num1 ^= len_num2;
    len_num2 ^= len_num1;
    len_num1 ^= len_num2;
  }
  // Make sure result is long enough.
  if (result->bn_size <= len_num1*len_num2) {
    bn_resize(result, len_num1*(len_num2 + 1));
  }
  result->bn_len = len_num1*(len_num2 + 1);

  int i;
  for (i = 0; i < len_num1; i++) {
    uint64_t carry = 0;
    int j;
    for (j = 0; j < len_num2; j++) {
      uint64_t re = ((uint64_t)num1->bn_data[i] * (uint64_t)num2->bn_data[j]) + result->bn_data[i + j] + carry;
      carry = re / 0x10000;
      result->bn_data[i + j] = re % 0x10000;
    }

    if (carry != 0) {
      result->bn_data[i + len_num2] += carry;
    }
  }
  result->bn_len = bn_reallen(result);
  bn_copy_shallow(result, re);
  return 0;
}


/**
 * Converts a number string to bn.
 *
 * @param bn: initialized bn_t to store the results into.
 * @param s: string to be converted.
 * @return 0 if successful, -1 otherwise.
 */
int bn_fromString(bn_t bn, const char *s) {
  // Make bn zero and create a ten bn.
  bn_make_zero(bn);
  bn_t ten = bn_create_int(10);

  int i;
  for (i = 0; s[i] != '\0'; i++) {
    uint16_t d = s[i] - '0';
    if (d > 9) return -1;
    bn_t bn_digit = bn_create_int(d);

    bn_mul(bn, bn, ten);

    bn_add(bn, bn, bn_digit);

  char* str = malloc(sizeof(char)*50);
  bn_toString(bn, str, 50);
  }
  return 0;
}

int bn_IAmAnUndergrad() {
  return 1;
}
