#include "stack.h"

struct N {
    char str[STRING_CAPACITY];
};

struct S {
    StackNode* values;
    int capacity;
    int size;
};

bool isEmpty(Stack stack) {
    return stack->size < 1;
}

bool isAtCapacity(Stack stack) {
    return stack->size == stack->capacity;
}

bool isFull(Stack stack) {
    return stack->size >= MAXIMUM_CAPACITY;
}

int size(Stack stack) {
    return stack->size;
}

StackResponse createStack() {
    Stack stack = malloc(sizeof(struct S));
    if (stack == NULL) {
        return (StackResponse) {NULL, out_of_memory};
    }
    stack->values = malloc(STARTING_CAPACITY * sizeof(StackNode));
    if (stack->values == NULL) {
        free(stack);
        return (StackResponse) {NULL, out_of_memory};
    }
    return (StackResponse) {stack, success};
}

response_code push(Stack stack, const char* str) {
    if (stack == NULL) {
        return no_stack;
    }
    if (isFull(stack)) {
        return stack_full;
    }
    if (memchr(str, '\0', STRING_CAPACITY) == NULL) {
        return string_too_long;
    }
    if (isAtCapacity(stack)) {
        stack->capacity += STARTING_CAPACITY;
        stack->values = realloc(stack->values, stack->capacity * sizeof(StackNode));
        if (stack->values == NULL) {
            return out_of_memory;
        }
    }

    StackNode new_node = malloc(sizeof(struct N));
    if (new_node == NULL) {
        return out_of_memory;
    }
    strncpy(new_node->str, str, STRING_CAPACITY);
    stack->values[stack->size] = new_node;
    stack->size++;
    return success;
}

StringResponse pop(Stack stack) {
    if (stack == NULL) {
        return (StringResponse) {NULL, no_stack};
    }
    if (isEmpty(stack)) {
        return (StringResponse) {NULL, stack_empty};
    }
    const int top_index = stack->size - 1;
    const StackNode top_value = stack->values[top_index];
    stack->values[top_index] = NULL;
    stack->size--;
    return (StringResponse) {top_value->str, success};
}

StringResponse peek(Stack stack) {
    if (stack == NULL) {
        return (StringResponse) {NULL, no_stack};
    }
    return isEmpty(stack) 
    ? (StringResponse) {NULL, stack_empty} 
    : (StringResponse) {stack->values[stack->size - 1]->str, success};
}

response_code freeStack(Stack* stack) {
    if (stack == NULL || *stack == NULL) {
        return no_stack;
    }
    while ((*stack)->size > 0) {
        free((*stack)->values[(*stack)->size - 1]);
        (*stack)->size--;
    }
    free(*stack);
    *stack = NULL;
    return success;
}