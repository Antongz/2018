#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bn.h"
#include "bn_stack.h"

#define DEFAULT_SIZE 1024

//add a new number into stack
int add_num(bn_s stack, const char *s){
  bn_t a = bn_alloc();
  if(a == NULL){
    fprintf(stderr,"Error: failed to allocate new bignum\n");
    return -1;
  }
  if(bn_fromString(a, s) == -1){
    fprintf(stderr,"Error: failed to read number %s\n", s);
    return -1;
  }
  if(stack_push(stack, a) == -1){
    fprintf(stderr,"Error: failed to push bignum in stack\n");
    return -1;
  }
  return 0;
}

//calculate and push back the result
//if calculate has any error, push back the original number
int calc(bn_s stack, const char *op){
  bn_t a = stack_top(stack);
  if(a == NULL){
    fprintf(stderr,"Error: don't have enough number for calculate\n");
    return -1;
  }
  stack_pop(stack);
  bn_t b = stack_top(stack);
  if(b == NULL){
    stack_push(stack, a);
    fprintf(stderr,"Error: don't have enough number for calculate\n");
    return -1;
  }
  stack_pop(stack);
  bn_t result = bn_alloc();
  if(strcmp(op, "+") == 0){
    if(bn_add(result, a, b) == -1){
      stack_push(stack, b);
      stack_push(stack, a);
      fprintf(stderr,"Error: failed to calculate sum\n");
      return -1;
    }
  }
  else if(strcmp(op, "-") == 0){
    if(bn_sub(result, b, a) == -1){
      stack_push(stack, b);
      stack_push(stack, a);
      fprintf(stderr,"Error: failed to calculate difference\n");
      return -1;
    }
  }
  else if(strcmp(op, "*") == 0){
    if(bn_mul(result, a, b) == -1){
      stack_push(stack, b);
      stack_push(stack, a);
      fprintf(stderr,"Error: failed to calculate product\n");
      return -1;
    }
  }
  stack_push(stack, result);
  return 0;
}

//function for dup
int calc_dup(bn_s stack){
  bn_t a = stack_top(stack);
  if(a == NULL){
    fprintf(stderr,"Error: stack is empty, can't dup\n");
    return -1;
  }
  if(stack_push(stack, a) == -1){
    fprintf(stderr,"Error: failed to dup\n");
    return -1;
  }
  return 0;
}

//function for pop
int calc_pop(bn_s stack){
  if(stack_pop(stack) == -1){
    fprintf(stderr,"Error: stack is empty, can't pop\n");
    return -1;
  }
  return 0;
}

//function for print. Buffer size is dynamic
int calc_print(bn_s stack){
  bn_t a = stack_top(stack);
  if(a == NULL){
    fprintf(stderr,"Error: stack is empty, can't print\n");
    return -1;
  }
  char *buf = (char*)malloc(sizeof(char*)*DEFAULT_SIZE);
  int status = bn_toString(a, buf, DEFAULT_SIZE);
  while(status != 0){
    if(status == -1){
      fprintf(stderr,"Error: failed to convert bignum to string\n");
      return -1;
    }
    buf = (char*)realloc(buf,sizeof(char*)*status);
    status = bn_toString(a, buf, status);
  }
  printf("%s\n", buf);
  free(buf);
  return 0;
}

//function for swap.
int calc_swap(bn_s stack){
  bn_t a = stack_top(stack);
  if(a == NULL){
    fprintf(stderr,"Error: don't have enough number for swap\n");
    return -1;
  }
  stack_pop(stack);
  bn_t b = stack_top(stack);
  if(b == NULL){
    stack_push(stack, a);
    fprintf(stderr,"Error: don't have enough number for swap\n");
    return -1;
  }
  stack_pop(stack);
  stack_push(stack, a);
  stack_push(stack, b);
  return 0;
}

//function for dump
int calc_dump(bn_s stack){
  if(stack_print(stack) == -1)
    return -1;
  return 0;
}

void calc_clear(bn_s stack){
  stack_clear(stack);
}

//main function
//read stdin via getline() because getline() has dynamic buffer size
//use strtok to split input line
//use strspn to check the each char of token is digit 
int main(){
  bn_s stack = init_stack();
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, stdin)) != -1){
    char* token = strtok(line,"\n");
    token = strtok(token," ");
    while(token){
      if(strcmp(token, "+")==0 ||
         strcmp(token, "-")==0 ||
         strcmp(token, "*")==0){
        calc(stack, token);
      }
      else if(strcmp(token, "dup")==0){
        calc_dup(stack);
      }
      else if(strcmp(token, "pop")==0){
        calc_pop(stack);
      }
      else if(strcmp(token, "print")==0){
        calc_print(stack);
      }
      else if(strcmp(token, "swap")==0){
        calc_swap(stack);
      }
      else if(strcmp(token, "dump")==0){
        calc_dump(stack);
      }
      else if(strcmp(token, "clear")==0){
        calc_clear(stack);
      }
      else if(strspn(token, "0123456789")==strlen(token)){
        add_num(stack,token);
      }
      else{
        fprintf(stderr,"Error: undefined word %s\n", token);
      }
      token = strtok(NULL," ");
    }
  }
  free(line);
  return 0;
}
