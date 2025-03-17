#include <stdlib.h>
#include <stdio.h> // TODO: REMOVE THIS LINE
#include <string.h>
#include <stdbool.h>

#ifndef STACK_H
#define STACK_H

#define MAXIMUM_CAPACITY 65536
#define STARTING_CAPACITY 16

#define STRING_CAPACITY 16

typedef struct N* StackNode;
typedef struct S* Stack;

typedef enum {
    success,
    out_of_memory,
    string_too_long,
    stack_full,
    stack_empty,
    no_stack
} response_code;

typedef struct {
    const Stack stack;
    response_code code;
} StackResponse;

typedef struct {
    const char* str;
    response_code code;
} StringResponse;

bool isEmpty(Stack stack);
bool isAtCapacity(Stack stack);
bool isFull(Stack stack);
int size(Stack stack);

StackResponse createStack();
response_code push(Stack stack, const char* str);
StringResponse pop(Stack stack);
StringResponse peek(Stack stack);
response_code freeStack(Stack* stack);

#endif