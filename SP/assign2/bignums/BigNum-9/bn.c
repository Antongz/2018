#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "bn.h"

int bn_IAmAnUndergrad(){
    return 1;
}

int MAX(int a, int b){
    if (a>b) {
        return a;
    }
    return b;
}

int MIN(int a, int b){
    if (a<b) {
        return a;
    }
    return b;
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
  int j;
  for (j = 0; j < bn->bn_len; j++) {
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
  int i;
  for (i = binlen; i--; ) {
    dbn_push(dbn, bn->bn_data[i] >> 8);
    dbn_push(dbn, bn->bn_data[i] & 0xFF);
  }
  return dbn;
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

uint16_t get_data(int i, bn_t a){
    if (i>a->bn_len-1) {
        return 0;
    }
    return a->bn_data[i];
}

int bn_add(bn_t result, bn_t a, bn_t b){
    int i = 0, tmpa = 0, tmpb = 0, carry = 0, tmpres = 0;
    int maxlen = MAX(a->bn_len,b->bn_len);
    if (bn_resize(result,maxlen+1) == -1) {
        return -1;
    }
    result->bn_len = maxlen;
    //printf("maxlen: %d\n", maxlen);
    for (i = 0; i < maxlen; i++) {
        tmpa = get_data(i,a);
        tmpb = get_data(i,b);
        tmpres = tmpa + tmpb + carry;
        //printf("%d: %d + %d + %d = %d", i, tmpa, tmpb, carry, tmpres);
        if (USHRT_MAX > tmpres) {
            result->bn_data[i] = (uint16_t)tmpres;
            carry = 0;
        }else{
            tmpres = tmpres - USHRT_MAX - 1;
            result->bn_data[i] = (uint16_t)tmpres;
            carry = 1;
            if (i == maxlen-1) {
                result->bn_len++;
                result->bn_data[i+1] = carry;
            }
        }
        //printf(" | %d ? %d\n", result->bn_data[i], carry);
    }
    result->bn_len = bn_reallen(result);
    return 0;
}

int is_valid_sub(bn_t result, bn_t a, bn_t b){
    int i = 0, tmpa = 0, tmpb = 0;
    if (a->bn_len < b->bn_len) {
        return 1;
    }
    //printf("\n");
    int maxlen = MAX(a->bn_len,b->bn_len);
    for (i = maxlen-1; i >= 0; i--) {
        tmpa = get_data(i,a);
        tmpb = get_data(i,b);
        //printf("%d: %d ? %d\n", i, tmpa, tmpb);
        if (tmpa < tmpb) {
            return 1;
        }else if (tmpa > tmpb) {
            return 0;
        }
    }
    return 0;
}

int bn_sub(bn_t result, bn_t a, bn_t b){
    if (is_valid_sub(result, a, b)) {
        bn_fromString(result,"0");
        return 0;
    }

    int i = 0, tmpa = 0, tmpb = 0, tmpres = 0;
    int maxlen = a->bn_len;
    if (bn_resize(result,maxlen) == -1) {
        return -1;
    }
    result->bn_len = maxlen;
    //printf("maxlen: %d\n", maxlen);

    for (i = 0; i < maxlen; i++) {
        tmpa = get_data(i,a);
        tmpb = get_data(i,b);
        tmpres = tmpa - tmpb;

        //printf("%d: %d - %d = %d", i, tmpa, tmpb, tmpres);
        if (tmpres >= 0) {
            result->bn_data[i] = tmpres;
        }else{
            tmpres = USHRT_MAX + 1 + tmpa - tmpb;
            result->bn_data[i] = tmpres;
            int j = 0;
            while (1) {
                j++;
                if (a->bn_data[i+j] == 0) {
                    a->bn_data[i+j] = USHRT_MAX;
                }else{
                    a->bn_data[i+j] -= 1;
                    break;
                }
            }
        }
        //printf(" | %d\n", result->bn_data[i]);
    }
    result->bn_len = bn_reallen(result);
    return 0;
}

void printbn(bn_t a){
    int i;
    for (i = 0; i < a->bn_len; i++) {
        printf("%d ", a->bn_data[i]);
    }
    printf("\n");
}

int shift(int value, bn_t a){
    a->bn_len = bn_reallen(a);
    //printf("\n");
    // printbn(a);
    if (value == 0) {
        // printf("no shift\n");
        return 0;
    }
    int initsize = a->bn_len;
    // printf("%d\n", initsize);
    uint16_t *tmp = (uint16_t *)malloc(initsize * sizeof(uint16_t));
    // printf("shift\n");
    memcpy(tmp, a->bn_data, initsize * sizeof(uint16_t));
    bn_resize(a,a->bn_len+value);
    a->bn_len = a->bn_len+value;
    int i;
    for (i = 0; i < value; i++) {
        a->bn_data[i] = 0;
    }
    for (i = 0; i < initsize; i++) {
        a->bn_data[i+value] = tmp[i];
    }
    // memset(tmp, 0, sizeof(tmp));
    free(tmp);
    // printbn(a);
    //printf("shift: %d ? %d | %d\n\n", value, initsize, a->bn_len);
    return 0;
}

void printtostring(bn_t a){
    char buf[USHRT_MAX];
    bn_toString(a, buf, sizeof(buf));
    printf("%s\n", buf);
}

void bn_copy(bn_t result, bn_t a) {
    bn_resize(result, a->bn_size);
    memcpy(result->bn_data, a->bn_data, sizeof(uint16_t)*a->bn_size);
    result->bn_sign = a->bn_sign;
    result->bn_len = a->bn_len;
}

int bn_mul(bn_t result, bn_t a, bn_t b){
    uint16_t *adata = a->bn_data;
    uint16_t *bdata = b->bn_data;
    int alen = a->bn_len;
    int blen = b->bn_len;
    int i, j, reslen = alen+blen, carry = 0, tcarry = 0, base = 65536, k = 0;
    uint32_t *res = (uint32_t *)malloc(sizeof(uint32_t) * reslen);
    if(res == NULL){
        return -1;
    }
    for(i = 0; i < reslen; i++){
        res[i] = 0;
    }
    uint32_t tempres = 0;
    for(i = 0; i < blen; i++){
        carry = 0;
        for(j = 0; j < alen; j++){
            tcarry = 0;
            res[j+k] = res[j+k]+(bdata[i]*adata[j])+carry;
            if(res[j+k] >= base){
                tempres = res[j+k];
                res[j+k] = tempres%base;
                tcarry = tempres/base;
                if(j == alen-1){
                    res[j+k+1] = res[j+k+1]+tcarry;
                }
            }
            carry = tcarry;
        }
        k = k+1;
    }
    if(bn_resize(result,reslen) < 0){
        return -1;
    }
    result->bn_len = reslen;
    for(i = 0; i < reslen; i++){
        result->bn_data[i] = res[i];
    }

    result->bn_len = bn_reallen(result);
    return 0;
}

uint16_t mod(const char *str,int maxshort){
    uint16_t res = 0, i = 0;
    for (i = 0; i < strlen(str); i++) {
        res = (res * 10 + (uint16_t)str[i] - '0') % maxshort;
    }
    return res;
}

int longDivision(char* result, const char number[], int divisor){
    int j = 0;
    int temp = number[j] - '0';
    while (temp < divisor){
       temp = temp * 10 + (number[++j] - '0');
    }
    long i = -1;
    while (strlen(number) > j){
        i++;
        result[i] = (temp / divisor) + '0';
        temp = (temp % divisor) * 10 + number[++j] - '0';
    }
    if (strlen(result) == 0){
        result = "0";
        return -1;
    }
    result[i+1] = '\0';
    return 0;
}

int comp(char str[]){
    int maxlen = strlen(str);
    if (maxlen > 5) {
        return 1;
    }
    int tmp = atoi(str);
    if (tmp > USHRT_MAX) {
        return 1;
    }
    return 0;
}

int bn_fromString(bn_t bn, const char *str){
    int maxlen = strlen(str);
    int i = 0;
    uint16_t remain = 0;
    char quotient[maxlen];
    int maxshort = (int)USHRT_MAX+1;

    bn_resize(bn, maxlen);
    bn->bn_len = maxlen;

    remain = mod(str,maxshort);
    longDivision(quotient, str, maxshort);
    //printf("%s | %d\n", quotient, remain);
    bn->bn_data[i] = remain;
    while (comp(quotient)) {
        i++;
        remain = mod(quotient,maxshort);
        bn->bn_data[i] = remain;
        longDivision(quotient, quotient, maxshort);
        //printf("%s | %d\n", quotient, remain);
    }
    i++;
    remain = mod(quotient,maxshort);
    //printf("0 | %d\n", remain);
    bn->bn_data[i] = remain;
    bn->bn_len = bn_reallen(bn);
    //printbn(bn);
    //printtostring(bn);
    // printf("%s %d\n", quotient, remain);
    return 0;
}
