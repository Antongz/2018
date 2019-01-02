#include <stdio.h>
#include <stdlib.h>

#include "bn_stack.h"

#define DEFAULT_SIZE 1024

//Implement a bignum stack base on Linked list

struct bn_stack {
  bn_t data;
  bn_s next;
};

//allocate memory for stack head
bn_s init_stack(){
  bn_s stack = (bn_s)malloc(sizeof(struct bn_stack));
  if(stack == NULL)
    return NULL;
  stack->next = NULL;
  return stack;
}

//free the stack
void stack_free(bn_s stack){
  stack_clear(stack);
  free(stack);
}

//push a bignum into stack
int stack_push(bn_s stack, bn_t bn){
  bn_s node = init_stack();
  if(node == NULL){
    return -1;
  }
  node->data = bn;
  node->next = stack->next;
  stack->next = node;
  return 0;
}

//pop the top node from stack
int stack_pop(bn_s stack){
  bn_s node = stack->next;
  if(node == NULL){
    return -1;
  }
  stack->next = node ->next;
  free(node);
  return 0;
}

//return the bignum in the top of stack
bn_t stack_top(bn_s stack){
  bn_s node = stack->next;
  if(node == NULL){
    return NULL;
  }
  return node->data;
}

//print all bignum in the stack
int stack_print(bn_s stack){
  bn_s p = stack->next;
  while(p!=NULL){
    char *buf = (char*)malloc(sizeof(char*)*DEFAULT_SIZE);
    int status = bn_toString(p->data, buf, DEFAULT_SIZE);
    while(status != 0){
      if(status == -1){
        fprintf(stderr,"Error: failed to convert bignum to string\n");
        return -1;
      }
      buf = (char*)realloc(buf,sizeof(char*)*status);
      status = bn_toString(p->data, buf, status);
    }
    printf("%s\n", buf);
    free(buf);
    p = p->next;
  }
  return 0;
}

//clear the stack
void stack_clear(bn_s stack){
  while(stack->next != NULL){
    bn_free(stack_top(stack));
    stack_pop(stack);
  }
}
