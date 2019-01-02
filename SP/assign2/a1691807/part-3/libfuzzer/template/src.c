#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef uint16_t bn_digit;
typedef uint32_t bn_intermediate;

struct bn_t {
    bn_digit* data;
    size_t size;
    size_t length;
};

typedef struct bn_t bn;

bn* bn_alloc(){
    bn* num = malloc(sizeof(bn));

    num->data = NULL;
    num->size = 0;
    num->length = 0;

    return num;
}

int bn_resize(bn* num, size_t sz){
    if(num->size >= sz){
        return 0;
    }

    num->data = realloc(num->data, sizeof(bn_digit) * sz);

    size_t start = num->size == 0 ? 0 : num->size - 1;

    for(size_t i = start; i < sz; i++){
        num->data[i] = 0;
    }

    num->size = sz;

    return 0;
}

int bn_free(bn* num){
    if(num->data != NULL){
        free(num->data);
    }
    free(num);
    return 0;
}

size_t bn_get_length(bn* num){
    size_t i = num->size - 1;

    do{
        if(num->data[i] != 0){
            return i + 1;
        }
    }
    while(i-- > 0);

    return 0;
}

int bn_add(bn* result, bn* a, bn* b){
    size_t sz = (a->length > b->length ? a->length : b->length) + 1;
    bn_digit* temp = malloc(sz);

    if(a->data == NULL){return -1;}
    if(b->data == NULL){return -1;}

    bn_intermediate value = 0;
    for(size_t i = 0; i < sz; i++){
        value += a->data[i] + b->data[i];
        temp[i] = (value % 10);
        value = value / 10;
    }

    result->data = temp;
    result->size = sz;
    result->length = bn_get_length(result);

    return 0;
}

int bn_from_string(bn* num, char* buffer){
    size_t n = 0;
    size_t b = strlen(buffer);
    bn_resize(num, b);

    do{
        num->data[n++] = buffer[--b] - '0';
    }while(b > 0);

    num->length = bn_get_length(num);

    return 0;
}

int bn_to_string(char* buffer, size_t size, bn* num){
    size_t b = 0;
    size_t n = num->length;

    do{
        buffer[b++] = num->data[--n] + '0';
    }while(b < size && n > 0);
    buffer[b] = 0;

    return 0;
}

struct calc_t {
    bn** stack;
    size_t capacity;
    size_t size;
};

typedef struct calc_t calc;

calc* calc_alloc(){
    calc* c = malloc(sizeof(calc));

    c->stack = malloc(sizeof(bn*) * 32);
    c->capacity = 32;
    c->size = 0;

    return c;
}

bn* calc_pop(calc* c){
    return c->stack[--c->size];
}

int calc_push(calc* c, bn* num){
    c->stack[c->size++] = num;
    return 0;
}

int parse(calc* c, char* buffer){
    size_t sz = strlen(buffer);
    size_t i = 0;

    while(i < sz){
        if(isdigit(buffer[i])){
            size_t end = i;
            while(isdigit(buffer[++end])){}
            char temp = buffer[end];
            buffer[end] = 0;

            bn* num = bn_alloc();
            bn_from_string(num, buffer + i);
            calc_push(c, num);

            i = end;
            buffer[end] = temp;
        } else if(buffer[i] == '+') {
            i++;

            bn* a = calc_pop(c);
            bn* b = calc_pop(c);
            bn* result = bn_alloc();

            bn_add(result, a, b);

            bn_free(a);
            bn_free(b);
            calc_push(c, result);
        } else if(strncmp(buffer + i, "print", 5) == 0) {
            i += 5;

            bn* num = calc_pop(c);
            size_t sz = sizeof(char) * num->length + 1;
            char* buffer = malloc(sz);
            bn_to_string(buffer, sz, num);

            printf("%s\n", buffer);
            bn_free(num);
            free(buffer);
        } else {
            i++;
        }
    }
    return 0;
}

