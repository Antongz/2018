#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "bn.h"

struct Node {
  bn_t data;
  struct Node *next;
};

struct Node *new_node(bn_t bn) {
  struct Node *node = (struct Node *)malloc(sizeof(struct Node));
  node->data = bn;
  node->next = NULL;
  return node;
}

struct Node *root = NULL;

static int parse(char *op) {
  char buf[100000];
  if(isdigit(op[0]) != 0) {
    for(int i = 1; i < strlen(op); i++) {
      if(isdigit(op[i] == 0)) {
        return -1;
      }
    }
    bn_t bn_new = bn_alloc();
    if(bn_fromString(bn_new, op) != 0) {
      return -1;
    }
    struct Node *node = new_node(bn_new);
    node->next = root;
    root = node;
    return 0;
  }
  else if(strcmp("+", op) == 0) {
    bn_t result = bn_alloc();
    if(root == NULL || root->next == NULL) {
      return -1;
    }
    if(bn_add(result, root->data, root->next->data) != 0) {
      return -1;
    }
    struct Node *temp1 = root;
    struct Node *temp2 = root->next;
    root = root->next->next;
    bn_free(temp1->data);
    bn_free(temp2->data);
    free(temp1);
    free(temp2);
    struct Node *node = new_node(result);
    node->next = root;
    root = node;
    return 0;
  }
  else if(strcmp("-", op) == 0) {
    bn_t result = bn_alloc();
    if(root == NULL || root->next == NULL) {
      return -1;
    }
    if(bn_sub(result, root->next->data, root->data) != 0) {
      return -1;
    }
    struct Node *temp1 = root;
    struct Node *temp2 = root->next;
    root = root->next->next;
    bn_free(temp1->data);
    bn_free(temp2->data);
    free(temp1);
    free(temp2);
    struct Node *node = new_node(result);
    node->next = root;
    root = node;
    return 0;
  }
  else if(strcmp("*", op) == 0) {
    bn_t result = bn_alloc();
    if(root == NULL || root->next == NULL) {
      return -1;
    }
    if(bn_mul(result, root->data, root->next->data) != 0) {
      return -1;
    }
    struct Node *temp1 = root;
    struct Node *temp2 = root->next;
    root = root->next->next;
    bn_free(temp1->data);
    bn_free(temp2->data);
    free(temp1);
    free(temp2);
    struct Node *node = new_node(result);
    node->next = root;
    root = node;
    return 0;
  }
  else if(strcmp("dup", op) == 0) {
    bn_t bn_dup = bn_alloc();
    if(root == NULL) {
      return -1;
    }
    if(bn_toString(root->data, buf, sizeof(buf)) != 0) {
      return -1;
    }
    if(bn_fromString(bn_dup, buf) != 0) {
      return -1;
    }
    struct Node *node = new_node(bn_dup);
    node->next = root;
    root = node;
    return 0;
  }
  else if(strcmp("pop", op) == 0) {
    if(root != NULL) {
      struct Node *temp = root;
      root = root->next;
      bn_free(temp->data);
      free(temp);
      return 0;
    }
    else {
      return -1;
    }
  }
  else if(strcmp("print", op) == 0) {
    if(root == NULL) {
      return -1;
    }
    if(bn_toString(root->data, buf, sizeof(buf)) != 0) {
      return -1;
    }
    printf("%s\n", buf);
    return 0;
  }
  else if(strcmp("swap", op) == 0) {
    if(root == NULL || root->next == NULL) {
      return -1;
    }
    struct Node *temp_r = root;
    struct Node *temp_n = root->next->next;
    root = root->next;
    root->next = temp_r;
    root->next->next = temp_n;
    return 0;
  }
  else if(strcmp("dump", op) == 0) {
    struct Node *temp = root;
    while(temp != NULL) {
      if(bn_toString(temp->data, buf, sizeof(buf)) != 0) {
        return -1;
      }
      printf("%s\n", buf);
      temp = temp->next;
    }
    return 0;
  }
  else if(strcmp("clear", op) == 0) {
    while(root != NULL) {
      struct Node *temp = root;
      root = root->next;
      bn_free(temp->data);
      free(temp);
    }
    return 0;
  }
  else {
    return -1;
  }
}

int main() {
  char buf[10000];
  char *token;
  char delim[] = " \t\n\v\f\r";
  while(fgets(buf, sizeof(buf), stdin) != NULL) {
    token = strtok(buf, delim);
    while(token != NULL) {
      if(parse(token) == -1) {
        parse("clear");
        fprintf(stderr, "Error\n");
        return -1;
      }
      token = strtok(NULL, delim);
    }
  }
  parse("clear");
  return 0;
}
