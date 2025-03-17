#include <stdio.h>
#include <assert.h>
#include "stack.h"

int main() {
    StackResponse response = createStack();
    if (response.code != success) {
        return response.code;
    }
    puts("String stack created");

    Stack stack = response.stack;
    assert(push(stack, "!") == success);
    assert(push(stack, "World") == success);
    assert(push(stack, "Hello") == success);
    // Only allowing 16 bytes per string right now so the line below returns an error code
    assert(push(stack, "This should say \"Hello (newline) World (newline) !\" ********") == string_too_long);
    assert(!isEmpty(stack));

    assert(peek(stack).code == success);
    printf("Peeked Top of Stack: \"%s\"\n", peek(stack).str);

    StringResponse node = pop(stack);
    while (node.code == success) {
        node = pop(stack);
    }
    assert(node.code == stack_empty);
    assert(isEmpty(stack));
    
    // Cannot pop from an empty stack
    assert(pop(stack).code == stack_empty);

    // Test expandability of stack
    for (int i = 0; i < 64; i++) {
        char str[STRING_CAPACITY];
        snprintf(str, sizeof(str), "String %d", i);
        assert(push(stack, str) == success);
    }
    puts("64 Strings pushed");
    assert(!isEmpty(stack));
    assert(!isFull(stack));
    assert(peek(stack).code == success);
    printf("Peeked New Top of Stack: \"%s\"\n", peek(stack).str);

    int i = size(stack);
    while(!isFull(stack)) {
        char str[STRING_CAPACITY];
        snprintf(str, sizeof(str), "String %d", i++);
        assert(push(stack, str) == success);
    }
    puts("Stack is full");
    assert(peek(stack).code == success);
    printf("Peeked Top of Stack: \"%s\"\n", peek(stack).str);

    // Cannot perform operations on a freed stack
    assert(freeStack(&stack) == success);
    assert(stack == NULL);
    assert(push(stack, "Hello") == no_stack);
    assert(pop(stack).code == no_stack);
    assert(peek(stack).code == no_stack);

    puts("All tests passed");
}