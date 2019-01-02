//###The code below is written by Joshua Buditama (a1704937)###
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

int *toNumArray(const char *numstring, int maxlen)
{
  // printf("maxlenINT: %d\n",maxlen);
  int *numarr = malloc(sizeof (int) * maxlen);
  int length = (unsigned)strlen(numstring);
  // printf("lengthINT: %d\n",length);
  //initialising the array to 0's
  for(int idx = 0; idx < maxlen; idx++)
  {
    numarr[idx] = 0;
  }

  //converting the input string into workable number string
  // printf("numlength: %lu\n",sizeof(numarr)/sizeof(numarr[0]));
  for(int idx = 0; idx < length; idx++)
  {
    // printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    // printf("maxlenA: %d\n",numarr[maxlen-idx-1]);
    // printf("lengthA: %d\n",numstring[length-idx-1]-'0');
    if(numstring[length-idx-1] != 0)
    {
      numarr[maxlen-idx-1] = numstring[length-idx-1]-'0';
    }
    else
    {
      numarr[maxlen-idx-1] = numstring[length-idx-1];
    }
    // printf("maxlenB: %d\n",numarr[maxlen-idx-1]);
    // printf("lengthB: %d\n",numstring[length-idx-1]-'0');
  }

  return numarr;
}

char *string_sub(const char *num1, const char *num2)
{
  int firstlen = (unsigned)strlen(num1);
  int secondlen = (unsigned)strlen(num2);

  int maxlen = firstlen;

  if(maxlen < secondlen)
  {
    maxlen = secondlen;
  }
  // printf("num1: %s\n",num1);
  // printf("num2: %s\n",num2);
  int *firstnum = toNumArray(num1,maxlen);
  int *secondnum = toNumArray(num2,maxlen);
  // printf("FIRSTLEN: %d\n",firstlen);
  // for(int idx = 0; idx < sizeof(firstnum)/sizeof(firstnum); idx++)
  // {
  //   printf("NUM1: %d \n",firstnum[idx]);
  // }
  //Calculation section (school method)
  int base  = 10;
  int temp1;
  int temp2;
  int tempdiff;
  char diff[maxlen+1];
  diff[maxlen] = '\0';
  // int diffsize1 = (unsigned)strlen(diff);
  // printf("diffsize1: %d\n",diffsize1);
  for(int idx = maxlen-1; idx >= 0; idx--)
  {
    temp1 = firstnum[idx];
    temp2 = secondnum[idx];
    tempdiff = temp1-temp2;
    // printf("-------------------------------------------------------------------\n");
    // printf("idx: %d\n",idx);
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
  printf("diffbefore: %s | Length: %d\n",diff,(unsigned)strlen(diff));
  //avoiding unecessary leading 0'
  int startingidx = 0;
  int diffsize = (unsigned)strlen(diff);
  // printf("diffsize: %d\n",diffsize);
  if(diff[0] == '0')
  {
    int idx = 0;
    while(diff[idx] == '0' && idx < diffsize-1)
    {
      idx++;
      startingidx++;
    }
  }
  int size = (unsigned)strlen(diff)-startingidx;
  printf("SIZE: %d\n",size);
  char *res = malloc (sizeof (char) * size+1);
  res[size] = '\0';
  int idx = 0;
  while(startingidx+idx < diffsize)
  {
    res[idx] = diff[startingidx+idx];
    idx++;
  }
  // printf("DIFF: %s\n",res);
  int ressize = (unsigned)strlen(res);
  return res;
}

int numstring_cmp(const char *num1, const char *num2)
{
  int len1 = (unsigned)strlen(num1);
  int len2 = (unsigned)strlen(num2);

  if(len1 > len2)
  {
    return 1;
  }
  else if(len2 > len1)
  {
    return -1;
  }
  else if(num1[0] > num2[0])
  {
    return 1;
  }
  else if(num1[0] < num2[0])
  {
    return -1;
  }
  else
  {
    int status = 0;
    for(int idx = 0; idx < len1; idx++)
    {
      if(num1[idx] > num2[idx])
      {
        status = 1;
        break;
      }
      else if(num1[idx] < num2[idx])
      {
        status = -1;
        break;
      }
    }
    // printf("STATUS: %d\n",status);
    return status;
  }
}
