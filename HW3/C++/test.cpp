#include "stack.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <cassert>

static void testStackCreation() {
    Stack stack;
    assert(stack.isEmpty());
    std::cout << "testStackCreation passed." << std::endl;
}

static void testStackPush() {
    Stack stack;
	for (unsigned int i = 0; i <= 10; ++i) {
        stack.push("test " + std::to_string(i));
	}
    assert(!stack.isEmpty());
    assert(stack.peek()->compare("test 10") == 0);
    std::cout << "testStackPush passed." << std::endl;
}

static void testStackPop() {
    Stack stack;
    for (unsigned int i = 0; i <= 10; ++i) {
        stack.push("test " + std::to_string(i));
    }
	while (!stack.isEmpty()) {
		std::unique_ptr<std::string> item = stack.pop();
		assert(item != nullptr);
		assert(item->compare("test " + std::to_string(stack.size())) == 0);
	}
    assert(stack.isEmpty());
    try {
        stack.pop();
    }
    catch (const std::underflow_error& e) {
        assert(std::string(e.what()) == "Stack is empty. Cannot pop items.");
    }
    std::cout << "testStackPop passed." << std::endl;
}

static void testStackIsFull() {
    Stack stack;
	while (!stack.isFull()) {
		stack.push("test");
	}
    assert(stack.isFull());
    std::cout << "testStackIsFull passed." << std::endl;
}

static void testStackExpandable() {
	Stack stack;
    int i = 0;
    while (!stack.isFull()) {
        stack.push("test " + std::to_string(i++));
    }
    assert(stack.size() == MAXIMUM_CAPACITY);
    assert(stack.size() > STARTING_CAPACITY);
	try {
		stack.push("overflow");
	}
	catch (const std::overflow_error& e) {
		assert(std::string(e.what()) == "Stack is full. Cannot push more items.");
	}
	std::cout << "testStackExpandable passed." << std::endl;
}

void main() {
    testStackCreation();
    testStackPush();
    testStackPop();
    testStackIsFull();
    std::cout << "All tests passed." << std::endl;
}