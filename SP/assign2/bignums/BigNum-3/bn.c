#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

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

void bn_free(bn_t bn) {
  free(bn->bn_data);
  free(bn);
}

int bn_add(bn_t result, bn_t a, bn_t b) {
  if(result == NULL || a == NULL || b == NULL) {
    return -1;
  }
  int len = (a->bn_len >= b->bn_len) ? a->bn_len : b->bn_len;
  result->bn_len = len+1;
  if(bn_resize(result, len+1) == -1) {
    return -1;
  }
  int add = 0;
  for(int i = 0; i < len; i++) {
    if(a->bn_len > i) {
      add += a->bn_data[i];
    }
    if(b->bn_len > i) {
      add += b->bn_data[i];
    }
    if(add >= 65536) {
      add -= 65536;
      result->bn_data[i] = add;
      add = 1;
    }
    else {
      result->bn_data[i] = add;
      add = 0;
    }
  }
  result->bn_data[len] = add;
  return 0;
}

int bn_sub(bn_t result, bn_t a, bn_t b) {
  if(result == NULL || a == NULL || b == NULL) {
    return -1;
  }
  int len = (a->bn_len >= b->bn_len) ? a->bn_len : b->bn_len;
  result->bn_len = len;
  if(bn_resize(result, len) == -1) {
    return -1;
  }
  int sub = 0;
  for(int i = 0; i < len; i++) {
    if(a->bn_len > i) {
      sub += a->bn_data[i];
    }
    if(b->bn_len > i) {
      sub -= b->bn_data[i];
    }
    if(sub < 0) {
      sub += 65536;
      result->bn_data[i] = sub;
      sub = -1;
    }
    else {
      result->bn_data[i] = sub;
      sub = 0;
    }
  }
  if(sub == -1) {
    for(int i = 0; i < len; i++) {
      result->bn_data[i] = 0;
    }
    result->bn_len = 1;
  }
  return 0;
}

int bn_mul(bn_t result, bn_t a, bn_t b) {
  if(result == NULL || a == NULL || b == NULL) {
    return -1;
  }
  int len = (a->bn_len >= b->bn_len) ? a->bn_len : b->bn_len;
  result->bn_len = len*2;
  if(bn_resize(result, len*2) == -1) {
    return -1;
  }
  uint32_t *temp = (uint32_t *)calloc(len*2, sizeof(uint32_t));
  if(temp == NULL) {
    return -1;
  }
  for(int i = 0; i < a->bn_len; i++) {
    for(int j = 0; j < b->bn_len; j++) {
      if(a->bn_data[i] == 0 || b->bn_data[j] == 0) {}
      else {
        temp[i+j] += a->bn_data[i] * b->bn_data[j];
        temp[i+j+1] += temp[i+j] / 65536;
        temp[i+j] = temp[i+j] % 65536;
      }
    }
  }
  for(int i = 0; i < 2*len-1; i++) {
    temp[i+1] += temp[i] / 65536;
    temp[i] = temp[i] % 65536;
    result->bn_data[i] = temp[i];
  }
  result->bn_data[2*len-1] = temp[2*len-1];
  free(temp);
  return 0;
}

int bn_fromString(bn_t bn, const char *s) {
  int l = strlen(s);
  if(bn == NULL || l == 0) {
    return -1;
  }
  uint32_t *arr = (uint32_t *)calloc(l, sizeof(uint32_t));
  if(arr == NULL) {
    return -1;
  }
  int number, temp;
  for(int i = 0; i < l; i++) {
    for(int j = 0; j < l; j++) {
      arr[j] *= 10;
    }
    number = s[i] - '0';
    temp = arr[0] + number;
    arr[0] = temp % 65536;
    if(l > 1) {
      arr[1] += temp / 65536;
      if(l > 2) {
        for(int j = 1; j < l-1; j++) {
          temp = arr[j];
          arr[j] = temp % 65536;
          arr[j+1] += temp / 65536;
        }
      }
    }
  }
  int len = l - 1;
  while(1) {
    if(arr[len] != 0) {
      break;
    }
    else if(len == 0) {
      break;
    }
    else {
      len--;
    }
  }
  len++;
  if(bn_resize(bn, len) == -1) {
    return -1;
  }
  bn->bn_len = len;
  for(int i = 0; i < len; i++) {
    bn->bn_data[i] = arr[i];
  }
  free(arr);
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

int bn_IAmAnUndergrad() {
  return 1;
}
