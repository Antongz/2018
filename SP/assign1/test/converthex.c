/*
 * =====================================================================================
 *
 *       Filename:  converthex.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/2018 09:10:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:  
 *
 * =====================================================================================
 */
/*  sprintf example */
#include <stdio.h>
#include <stdlib.h>

int main ()
{
      char buffer [1000];
      int no = 10;
//      sprintf (buffer, "%X", 50000000000000000000000000);
//      printf ("buffer is %s\n", buffer);

      char buf1[] = "240000000000000000000000000000000000000";
      char *pEnd;

      unsigned long long int ulli1;
      ulli1 = strtoull(buf1, NULL, 10);

      printf ("The unsigned decemal is : %llu\n\n", ulli1);

      return 0;
}

