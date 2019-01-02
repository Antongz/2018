#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bn.h"


int main(int argc, char *argv[]) {

  bn_t stack[1000];

  int sp = 0;

  char buffer[100000];

  while(fscanf(stdin, "%s", buffer) != EOF) {
    if( strcmp("dup", buffer) == 0 ) {
      if(sp < 1) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }
      char buf[1000000];
      if( bn_toString(stack[sp-1], buf, 1000000) == -1 ) {
        fprintf(stderr, "Error: toString Failed\n");
        exit(0);
      }

      stack[sp] = bn_alloc();

      if( bn_fromString(stack[sp], buf) == -1 ) {
        fprintf(stderr, "Error: fromString Failed\n");
        exit(0);
      }
      sp++;
    }

    else if( strcmp("+", buffer) == 0 ) {
      if(sp < 2) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }
      if( bn_add(stack[sp-2], stack[sp-2], stack[sp-1]) != 0) {
        fprintf(stderr, "Error: Add Failed\n");
        exit(0);
      }
      bn_free(stack[sp-1]);
      sp--;
    }


    else if( strcmp("*", buffer) == 0 ) {
      if(sp < 2) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }
      if( bn_mul(stack[sp-2], stack[sp-2], stack[sp-1]) != 0) {
        fprintf(stderr, "Error: Multiply Failed\n");
        exit(0);
      }
      bn_free(stack[sp-1]);
      sp--;

    }

    else if( strcmp("-", buffer) == 0 ) {
      if(sp < 2) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }

      if( bn_sub(stack[sp-2], stack[sp-2], stack[sp-1]) != 0 ) {
        fprintf(stderr, "Error: Subtract Failed\n");
        exit(0);
      }
      bn_free(stack[sp-1]);
      sp--;
    }

    else if( strcmp("pop", buffer) == 0 ) {
      if(sp < 1) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }      
      bn_free(stack[sp-1]);
      sp--;
    }

    else if( strcmp("print", buffer) == 0 ) {
      if(sp < 1) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }      
    	char buf[100000];
      	if( bn_toString(stack[sp-1], buf, sizeof(buf)) == -1) {
      		fprintf(stderr, "Error: toString failed\n");
          exit(0);
      	}
      	printf("%s\n", buf);
  	}

    else if( strcmp("swap", buffer) == 0 ) {
      if(sp < 2) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }
      char bn1[100000];
      if( bn_toString(stack[sp-1], bn1, 100000) == -1 ) {
        fprintf(stderr, "Error: toString Failed\n");
        exit(0);
      }

      char bn2[100000];
      if( bn_toString(stack[sp-2], bn2, 100000) == -1 ) {
        fprintf(stderr, "Error: toString Failed\n");
        exit(0);
      }

      bn_free(stack[sp-1]);
      bn_free(stack[sp-2]);
      sp = sp - 2;

      stack[sp] = bn_alloc();
      stack[sp+1] = bn_alloc();

      if( bn_fromString(stack[sp], bn1) == -1 ) {
        fprintf(stderr, "Error: fromString failed!\n");
        exit(0);
      }

      if( bn_fromString(stack[sp+1], bn2) == -1 ) {
        fprintf(stderr, "Error: fromString failed!\n");
        exit(0);
      }

      sp = sp + 2;
    }

    else if( strcmp("clear", buffer) == 0 ) {
      while(sp > 0) {
        bn_free(stack[sp-1]);
        sp--;
      }
    }

    else if( strcmp("dump", buffer) == 0 ) {
      if(sp < 1) {
        fprintf(stderr, "Error: Not Enough Numbers on the Stack\n");
        exit(0);
      }
      for(int i = sp ; i > 0 ; i--) {
        char buffer[100000];
        if( bn_toString(stack[i-1], buffer, 100000) == -1) {
          fprintf(stderr, "Error: toString failed\n");
          exit(0);
        }
        printf("%s\n", buffer);

      }
    }

    else {
      stack[sp] = bn_alloc();
      if( bn_fromString(stack[sp], buffer) == -1 ) {
        fprintf(stderr, "Error: fromString failed!\n");
        exit(0);
      }
      sp++;

    }
  }

}
