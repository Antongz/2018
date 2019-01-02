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

void dump(uint16_t *buf, int n) {
  for (int i = n; i--;) 
    printf("%04X", buf[i]);
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




// shift n digits of b left by d bits and store the result in a.
// Assumptions: 
//    d < 16
//    a and b have space for n digits
//    The caller will handle setting the length
// Returns the carry from the shift
static uint16_t shiftleft(uint16_t *a, uint16_t *b, int n, int d) {
  //printf("shiftleft(res, "); dump(b, n); printf(", %d, %d)\n", n, d);
  uint32_t carry = 0;
  for (int i = 0; i < n; i++) {
    carry |= ((uint32_t)b[i]) << d;
    a[i] = carry & 0xffff;
    carry >>= 16;
  }
  //printf("shiftleft: %04X  + ", carry); dump(a, n); printf("\n");
  return carry;
}
    

static uint32_t Step_D3(uint16_t *uj, uint16_t *v, int n) {
  uint32_t hat = (uj[n]<<16) + uj[n-1];
  uint32_t qhat = hat / v[n-1];
  uint32_t rhat = hat % v[n-1];
  if (qhat == 0x10000 || ( n>1 && (qhat * v[n-2] > 0x10000 * rhat + uj[n-2]))) {
    qhat--;
    rhat += v[n-1];
    if (rhat < 0x10000 && n>1 && (qhat * v[n-2] > 0x10000 * rhat + uj[n-2])) {
      qhat--;
      rhat += v[n-1];
    }
  }
  return qhat;
}

static uint16_t Step_D4(uint16_t *uj, uint16_t *v, uint32_t qhat, int n) {
  uint32_t borrow = 0;
  for (int i = 0; i < n; i++) {
    borrow += uj[i];
    borrow -= qhat * v[i];
    uj[i] = borrow & 0xFFFF;
    borrow >>= 16;
    if (borrow)
      borrow |= 0xFFFF0000; // The borrow is always non-positive...
  }
  borrow += uj[n];
  uj[n] = borrow & 0xFFFF;
  return borrow >> 16; // The return value is 16 bits, so no need for extending the sign bit
}

static void Step_D6(uint16_t *uj, uint16_t *v, int n) {
  uint32_t carry = 0;
  for (int i = 0; i < n; i++) {
    carry += uj[i];
    carry += v[i];
    uj[i] = carry & 0xFFFF;
    carry >>= 16;
  }
  carry += uj[n];
  uj[n] = carry & 0xFFFF;
  //assert(carry > 0xFFFF); // We ignore further carry
}

static void shiftright(uint16_t *u, int n, int d) {
  for (int i = 0; i < n; i++)
    u[i] = (u[i] >> d) | (u[i+1] << (16 - d));
  u[n] >>= d;
}




// Using Algorithm 4.3.1 D of Knuth TAOCP
int bn_div(bn_t quotient, bn_t remainder, bn_t numerator, bn_t denominator) {
  // Use Knuth's variable names:
  //   u -- numerator
  //   v -- denominator
  //   q -- quotient
  //   d -- normalisation factor
  //   n -- length of denominator
  //   m -- length difference between numerator and denominator
  //   b -- base = 0x10000
  // Our base (b) is 2^16, so we can use the shift method to calculate d.
  // We use the space of the remainder for the normalised numerator, so
  // need to allocate another variable for that.
  if (numerator->bn_sign < 0 || denominator->bn_sign < 0)
    return -1;
  if (quotient == numerator || 
      quotient == denominator || 
      quotient == remainder ||
      remainder == numerator ||
      remainder == denominator)
    return -1;

  // Step D1
  int n = bn_reallen(denominator);
  if (n == 0)
    return -1;
  int d = 0;
  uint16_t t = denominator->bn_data[n - 1];
  assert(t != 0); // This is OK from the calculation of n
  while ((t & 0x8000) == 0) {
    t <<= 1;
    d++;
  }
  bn_t vbn = bn_alloc();
  bn_resize(vbn, n);
  uint16_t *v = vbn->bn_data;
  t = shiftleft(v, denominator->bn_data, n, d);
  // Not setting len of vbn because we do not really use it.
  assert(t == 0);

  int nl =  bn_reallen(numerator);
  int m = nl < n ? 0 : nl - n;

  remainder->bn_len = n;
  bn_t ubn = remainder;
  bn_resize(ubn, m + n + 1);
  memset(ubn->bn_data, 0, (m + n + 1) * sizeof(uint16_t));
  uint16_t *u = ubn->bn_data;
  ubn->bn_data[nl] = shiftleft(u, numerator->bn_data, nl, d);

  bn_resize(quotient, m + 1);
  quotient->bn_len = m + 1;
  uint16_t *q = quotient->bn_data;


  // Steps D2, D7
  for (int j = m; j >= 0; j--) {
    // Step D3
    uint32_t qhat = Step_D3(u+j, v, n);

    // Step D4
    uint16_t borrow = Step_D4(u+j, v, qhat, n);
    
    // Step D5
    q[j] = qhat;
    if (borrow) {
      //Step D6
      assert(qhat != 0);
      Step_D6(u+j, v, n);
      q[j]--;
    }
  }
 
  // Step D8
  assert((u[0] & ((1<<d) - 1)) == 0);
  shiftright(u, n, d);
  bn_free(vbn);

  return 0;
}


