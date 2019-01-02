#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

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

bool bn_is_zero(bn_t bn) {
    bn->bn_len = bn_reallen(bn);
    if (bn->bn_len == 0 && bn->bn_data[0] == 0x00)
        return true;

    return false;
}

static void bn_srli(bn_t result, bn_t a, uint16_t shiftBit, int shift) {
    int i = 0;
    uint32_t carry = 0x00;
    uint32_t tempData = 0x00;

    for (i=a->bn_len - 1; i>=0; i--) {
        tempData = (uint32_t) ((carry) << 16);
        tempData = (uint32_t) (tempData | a->bn_data[i]);
        tempData >>= shift;

        carry = (uint32_t) (a->bn_data[i] & shiftBit);
        result->bn_data[i] = (uint16_t) tempData;
    }

    result->bn_len = bn_reallen(result);
}

// the destBN->array size should be the same as dataArray
int shadow_Copy(bn_t destBN, uint16_t *dataArray) {



    return 0;
}

// Computes base^(exp) mod modulus
// Use Right-to-left binary method: it has O(log(exponent)) Complexity.
int bn_modexp(bn_t result, bn_t base, bn_t exp, bn_t modulus) {
    bn_t tempResult = bn_alloc();
    if (tempResult == NULL) return -1;

    if (modulus->bn_len == 0 || (modulus->bn_len == 1 && modulus->bn_data[0] <= 1)) return -1;


    if (bn_is_zero(base)) {
        if (copyBN(result, tempResult) == -1) return -1;
        bn_free(tempResult);
        return 0;
    }

    if (bn_fromString(tempResult, "1") == -1) return -1;

    if (bn_is_zero(exp)) {
        if (copyBN(result, tempResult) == -1) return -1;
        bn_free(tempResult);
        return 0;
    }

    bn_t tempRemainder = bn_alloc();
    if (tempRemainder == NULL) return -1;
    bn_t tempBase = bn_alloc();
    if (tempBase == NULL) return -1;
    bn_t temp = bn_alloc();
    if (temp == NULL) return -1;

    if (bn_fromString(tempRemainder, "1") == -1) return -1;


//    base := base mod modulus
    {
     if (copyBN(tempBase, base) == -1) return -1;
     if (bn_div(temp, base, tempBase, modulus) == -1) return -1;

    }
    while (exp->bn_len > 0) {
//        if (exponent mod 2 == 1):
//            result := (result * base) mod modulus
//        exponent := exponent >> 1
//        base := (base * base) mod modulus
        if ((exp->bn_data[0] & 0x0001) == 0x0001) {
            if (bn_mul(tempResult, tempResult, base) == -1) return -1;
            if (bn_div(temp, tempRemainder, tempResult, modulus) == -1) return -1;
            if (copyBN(tempResult, tempRemainder) == -1) return -1;
        }

        bn_srli(exp, exp, 0x01, 1);
        {
            if (bn_mul(base, base, base) == -1) return -1;
            if (copyBN(tempBase, base) == -1) return -1;
            if (bn_div(temp, base, tempBase, modulus) == -1) return -1;
        }
    }

    if (copyBN(result, tempResult) == -1) return -1;
    bn_free(tempResult);
    bn_free(tempRemainder);
    bn_free(tempBase);
    bn_free(temp);

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
        result->bn_sign = -1;
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
                result->bn_sign = -1;
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
    for (int i=0; i<result->bn_len; i++) {
        result->bn_data[i] = dataArray[i];
    }
    free(dataArray);
//    result->bn_data = dataArray;
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

// Use bit shift to gain performance
int bn_sqr(bn_t result, bn_t a) {
    bn_t tempNum = bn_alloc();
    if (tempNum == NULL) return -1;
    bn_t tempRes = bn_alloc();
    if (tempRes == NULL) return -1;
    bn_t tempBit = bn_alloc();
    if (tempBit == NULL) return -1;

    // bit starts at the highest power of bit
    if (bn_resize(tempBit, a->bn_len) == -1) return -1;
    tempBit->bn_data[a->bn_len - 1] = (uint16_t)(0x4000);
    tempBit->bn_len = a->bn_len;

    if (copyBN(tempNum, a) == -1) return -1;

    bn_t tem = bn_alloc();
    if (tem == NULL) return -1;
    while (true) {
        if (copyBN(tem, tempBit) == -1) return -1;

        if (bn_sub(tem, tem, tempNum) == -1) return -1;
        if (tem->bn_len == 0)
            break;

        bn_srli(tempBit, tempBit, 0x0003, 2);
    }
    bn_free(tem);

    while (tempBit->bn_len > 0) {
        bn_t tem = bn_alloc();
        if (tem == NULL) return -1;
        bn_t tempAdd = bn_alloc();
        if (tempAdd == NULL) return -1;

        if (bn_add(tempAdd, tempRes, tempBit) == -1) return -1;
        if (bn_sub(tem, tempNum, tempAdd) == -1) return -1;

        if (tem->bn_sign > 0) {
            if (copyBN(tempNum, tem) == -1) return -1;
            bn_srli(tempRes, tempRes, 0x01, 1);
            if (bn_add(tempRes, tempRes, tempBit) == -1) return -1;
        }
        else
            bn_srli(tempRes, tempRes, 0x01, 1);
        bn_srli(tempBit, tempBit, 0x0003, 2);

        bn_free(tem);
        bn_free(tempAdd);
    }
    if (copyBN(result, tempRes) == -1) return -1;
    bn_free(tempRes);

    return 0;
}
