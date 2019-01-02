#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

int main() {
  //creates variable ch as a char
  char ch;
  int original;
  int decimal = 0;
  char letters[] = {'A','B','C','D','E','F'};


  //while loop continues as long as user doesn't input CTRL+d
  //converts decimal to hexadecimal

  while((ch = getchar()) !=EOF) {

    if(isdigit(ch)) {
    original = ch - '0';
    decimal = decimal * 10 + original;
    } if (ch == '\n') {
        break;
  }
  }

  int temp;
   while(decimal!=0) {
     temp = decimal % 16;
     if (temp>=0 && temp <10) {
       char print = temp + '0';
       putchar(print);

     }
     else {
       if (temp == 10) {
      putchar(letters[0]);
       }
     if(temp == 11){
       putchar(letters[1]);
     }
         if(temp==12) {
        putchar(letters[2]);
       }
     if(temp==13) {
         putchar(letters[3]);
         }
     if(temp==14) {
         putchar(letters[4]);
     }
     if(temp==15) {
        putchar(letters[5]);
     }
   }

   decimal = decimal / 16;
   }
   printf("\n");
  return 0;

}
