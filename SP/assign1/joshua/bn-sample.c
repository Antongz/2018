#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "bn.h"
#include "helper.h"

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

//###All of the code below this comment is written by Joshua Buditama (a1704937)###
void bn_free(bn_t bn)
{
  free(bn->bn_data);
  free(bn);
}

int bn_fromString(bn_t bn, const char *str)
{
  int bufsize = 100000;
  int numlen;
  char *tempstr = malloc(sizeof (char) * bufsize);
  // printf("here!!!!\n");
  memset(tempstr,'\0',sizeof(tempstr));
  strcpy(tempstr,str);
    // double newnum;
    // sscanf(input,"%lf",&newnum); //converting string to double

  // double tempnum = newnum;
  // double temptempnum = tempnum;
  uint16_t *numarr = bn->bn_data;
  //length of a power of a number, X^Y is ceil(Y log X)

  // int arrlen = 0;
  // int power = 0;
  // int currlen = arrlen;
  // int mult = 1;
  int maxpow = 0;
  int temppow = 0;
  char *difference;
  numlen = (unsigned)strlen(tempstr);
  while(temppow <= numlen) //finding the biggest power candidate
  {
    maxpow = maxpow + 1;
    temppow = ceil(maxpow*log10(pow(2,16)));
  }
  bn->bn_len = maxpow;
  bn->bn_size = maxpow;
  // printf("maxpow: %d\n",maxpow);
  maxpow = maxpow-1;
  for(int idx = 0; idx < maxpow+1; idx++)
  {
    numarr[idx] = 0;
    // printf("initial power: %d | multiplication: %d\n",idx,numarr[idx]);
  }
  numarr[maxpow] = 1;
  char bnbuf[bufsize];
  bn_toString(bn,bnbuf,bufsize);
  // printf("maxpow: %d\n",maxpow);
  // printf("--------------------------------------------------------------------\n");
  int temptemppow = 0;
  temppow = maxpow;
  int hasbeenreduced = 0;
  while(1)
  {
    printf("--------------------------------------------------------------------\n");
    printf("TEMPSTR: %s | Length: %d\n",tempstr,(unsigned)strlen(tempstr));
    printf("BNBUF: %s | Length: %d\n",bnbuf,(unsigned)strlen(bnbuf));
    printf("maxpow: %d\n",maxpow);
    printf("temppow: %d\n",temppow);
    // printf("numlen: %d\n",numlen);
    // if(numarr[temppow] != 1)
    // {
    //   temppow = temppow-1;
    //   numarr[temppow] = 1;
    //   bn->bn_len = bn->bn_len-1;
    // }
    if(numstring_cmp(tempstr,bnbuf) > 0) //if tempstr > bnbuf
    {
      printf("BIGGER!\n");
      while(numstring_cmp(tempstr,bnbuf) >= 0) //finding the biggest multiplication
      {
        numarr[temppow] = numarr[temppow] + 1;
        bn_toString(bn,bnbuf,bufsize);
        // printf("numarr[temppow]: %d\n",numarr[temppow]);
        // printf("tempstr: %s\n",tempstr);
        // printf("numstring_cmp(tempstr,bnbuf): %d\n",numstring_cmp(tempstr,bnbuf));
      }
      printf("numarr[temppow]: %d\n",numarr[temppow]);
      printf("bnbuf2!: %s | Lenght: %d\n",bnbuf,(unsigned)strlen(bnbuf));
    }
    else if(numstring_cmp(tempstr,bnbuf) == 0) //if tempstr == bnbuf
    {
      printf("EQUAL!\n");
      numarr[temppow] = numarr[temppow] + 1;
    }
    else //if tempstr < bnbuf
    {
      printf("SMALLER!\n");

      while(numstring_cmp(tempstr,bnbuf) < 0) //finding the biggest multiplication
      {
        // if(numstring_cmp(tempstr,str) == 0)
        // {
        if(!hasbeenreduced)
        {
          printf("hey\n");
          numarr[temppow] = 0;
          maxpow = maxpow-1;
        }
        if(temppow != 0)
        {
          temppow = temppow - 1;
        }
        numarr[temppow] = numarr[temppow]+1;
        // bn_toString(bn,bnbuf,bufsize);
        // }
        // else
        // {
          // numarr[temppow] = 0;
          // temppow = temppow-1;
          // numarr[temppow] = numarr[temppow]+1;
          if(bn->bn_len != 1)
          {
            bn->bn_len = bn->bn_len-1;
          }
          printf("bnlen: %d\n",bn->bn_len);
          bn_toString(bn,bnbuf,bufsize);
        // }
      }
      // bn->bn_len = maxpow;
      // bn->bn_size = maxpow;
      printf("BUF %s | Length: %d\n",bnbuf,(unsigned)strlen(bnbuf));
      printf("here!\n");
      printf("tempowHERE: %d\n",temppow);
      while(numstring_cmp(tempstr,bnbuf) >= 0) //finding the biggest multiplication
      {
        numarr[temppow] = numarr[temppow] + 1;
        bn_toString(bn,bnbuf,bufsize);
        // printf("bnbuf3!: %s\n",bnbuf);
      }
    }
    numarr[temppow] = numarr[temppow]-1;
    printf("nummarrtemppow: %d\n",numarr[temppow]);
    bn_toString(bn,bnbuf,bufsize);
    printf("FINTEMPSTR: %s | Length: %d\n",tempstr,(unsigned)strlen(tempstr));
    printf("FINBNBUF  : %s | Length: %d\n",bnbuf,(unsigned)strlen(bnbuf));
    difference = string_sub(tempstr,bnbuf);
    printf("difference %s | Length: %d\n",difference,(unsigned)strlen(difference));
    if(strcmp(difference,"0") == 0)
    {
      break;
    }
    tempstr = difference;
    numlen = (unsigned)strlen(tempstr);
    temppow = 0;
    temptemppow = 0;
    while(temptemppow <= numlen) //finding the next biggest power candidate
    {
      temppow = temppow + 1;
      temptemppow = ceil(temppow*log10(pow(2,16)));
    }
    bn->bn_len = temppow;
    temppow = temppow-1;
    bn_toString(bn,bnbuf,bufsize);
    hasbeenreduced = 1;
  }
  // double result = 0;
  //
  // while(temptempnum != 0) //finding the biggest power possible
  // {
  //   result = temptempnum-pow(pow(2,16),power);
  //   if(result < 0)
  //   {
  //     if(maxpow < power)
  //     {
  //       maxpow = power;
  //     }
  //     power = power-1;
  //     while(1) //finding the biggest multiple possible
  //     {
  //       temptempnum = tempnum;
  //       result = temptempnum-pow(pow(2,16),power)*mult;
  //       if(result < 0)
  //       {
  //         mult = mult-1;
  //         numarr[power] = mult;
  //         arrlen = arrlen + 1;
  //         temptempnum = tempnum-pow(pow(2,16),power)*mult;
  //         break;
  //       }
  //       mult = mult+1;
  //     }
  //   }
  //   if(currlen != arrlen)
  //   {
  //     currlen = arrlen;
  //     power = 0;
  //     mult = 1;
  //     tempnum = temptempnum;
  //     continue;
  //   }
  //   power = power+1;
  //   temptempnum = tempnum;
  // }
  printf("maxpow: %d\n",maxpow);
  bn->bn_size = maxpow+1;
  bn->bn_len = maxpow+1;
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  for(int idx = 0; idx < maxpow+1; idx++)
  {
    printf("power: %d | multiplication: %d\n",idx,numarr[idx]);
  }
  char finbuf[bufsize];
  bn_toString(bn,finbuf,bufsize);
  printf("FINAL STRING: %s | Length: %d\n",finbuf, (unsigned)strlen(finbuf));
  return 0;
}

int bn_add(bn_t result, bn_t a, bn_t b)
{
  int bufsize = 1000;
  char firstbuf[bufsize];
  char secondbuf[bufsize];
  bn_toString(a,firstbuf,bufsize);
  bn_toString(b,secondbuf,bufsize);
  int firstlen = (unsigned)strlen(firstbuf);
  int secondlen = (unsigned)strlen(secondbuf);
  int maxlen = firstlen;

  if(maxlen < secondlen)
  {
    maxlen = secondlen;
  }

  int *firstnum = toNumArray(firstbuf,maxlen+1);
  int *secondnum = toNumArray(secondbuf,maxlen+1);

  //Calculation section (school method)
  int carry = 0;
  int base  = 10;
  int temp1;
  int temp2;
  int tempsum;
  char total[maxlen+1];
  for(int idx = maxlen; idx >= 0; idx--)
  {
    temp1 = firstnum[idx];
    temp2 = secondnum[idx];
    tempsum = temp1+temp2;

    if(tempsum >= base)
    {
      tempsum = tempsum - base;
      total[idx] = (tempsum+carry)+'0';
      carry = 1;
    }
    else if(tempsum+carry >= base)
    {
      tempsum = tempsum - base;
      total[idx] = (tempsum + carry)+'0';
      carry = 1;
    }
    else
    {
      total[idx] = (tempsum+carry)+'0';
      carry = 0;
    }
  }

  //storing the addition result
  bn_fromString(result,total);

  return 0;
}
int bn_sub(bn_t result, bn_t a, bn_t b)
{
  if(a->bn_data >= b->bn_data)
  {
    bn_fromString(result,"0");
    return 0;
  }
  int bufsize = 1000;

  char firstbuf[bufsize];
  char secondbuf[bufsize];
  bn_toString(a,firstbuf,bufsize);
  bn_toString(b,secondbuf,bufsize);
  int firstlen = (unsigned)strlen(firstbuf);
  int secondlen = (unsigned)strlen(secondbuf);
  int maxlen = firstlen;

  if(maxlen < secondlen)
  {
    maxlen = secondlen;
  }

  int *firstnum = toNumArray(firstbuf,maxlen);
  int *secondnum = toNumArray(secondbuf,maxlen);
  // printf("firstnum %d %d\n",firstnum[0],firstnum[1]);
  //Calculation section (school method)
  int base  = 10;
  int temp1;
  int temp2;
  int tempdiff;
  char diff[maxlen];
  for(int idx = maxlen-1; idx >= 0; idx--)
  {
    temp1 = firstnum[idx];
    temp2 = secondnum[idx];
    tempdiff = temp1-temp2;
    // printf("-------------------------------------------------------------------\n");
    // printf("idx: %d\n",idx);
    // printf("maxlen: %d\n",maxlen);
    // printf("temp1: %d\n",temp1);
    // printf("temp2: %d\n",temp2);
    // printf("tempdiff: %d\n",tempdiff);
    if(tempdiff < 0)
    {
      firstnum[idx-1] = firstnum[idx-1]-1;
      tempdiff = tempdiff + base;
      diff[idx]= tempdiff+'0';
    }
    else
    {
      diff[idx] = tempdiff+'0';
    }
  }
  // printf("tempdiff: %c%c\n",diff[0],diff[1]);

  //storing the substraction result
  bn_fromString(result,diff);

  return 0;
}
int bn_IAmAnUndergrad(void)
{
  return 1;
}
