#include "stack.h"
#include <stdexcept>

#define MAXIMUM_STRING_LENGTH 16

namespace Validate {
	static void isValidString(const std::string& str) {
		if (str.empty()) {
			throw std::invalid_argument("String cannot be empty");
		}
		if (str.length() > MAXIMUM_STRING_LENGTH) {
			throw std::invalid_argument("String cannot be too long");
		}
	}
	static void isNotEmpty(const Stack& stack) {
		if (stack.isEmpty()) {
			throw std::underflow_error("Stack is empty");
		}
	}
	static void isNotFull(const Stack& stack) {
		if (stack.isFull()) {
			throw std::overflow_error("Stack is full");
		}
	}
}

Stack::Stack() : _size(0), _capacity(STARTING_CAPACITY) {
	values = new std::unique_ptr<std::string>[_capacity];
}

Stack::~Stack() {
	for (int i = 0; i < _size; ++i) {
		values[i].reset();
	}
	delete[] values;
}

Stack::Stack(Stack&& other) noexcept : values(other.values), _capacity(other._capacity), _size(other._size) {
	other.values = nullptr;
	other._capacity = 0;
	other._size = 0;
}

Stack& Stack::operator=(Stack&& other) noexcept {
	if (this != &other) {
		for (int i = 0; i < _size; ++i) {
			values[i].reset();
		}
		delete[] values;
		values = other.values;
		_capacity = other._capacity;
		_size = other._size;
		other.values = nullptr;
		other._capacity = 0;
		other._size = 0;
	}
	return *this;
}

unsigned int Stack::size() const {
	return _size;
}

bool Stack::isFull() const {
	return _size >= MAXIMUM_CAPACITY;
}

bool Stack::isAtCapacity() const {
	return _size >= _capacity;
}

bool Stack::isEmpty() const {
	return _size <= 0;
}

void Stack::push(std::string item) {
	Validate::isValidString(item);
	if (isAtCapacity()) {
		Validate::isNotFull(*this);
		_capacity *= 2;
		auto new_values = new std::unique_ptr<std::string>[_capacity];
		for (unsigned int i = 0; i < _size; ++i) {
			new_values[i] = std::move(values[i]);
		}
		delete[] values;
		values = new_values;
	}
	values[_size++] = std::make_unique<std::string>(item);
}

std::unique_ptr<std::string> Stack::pop() {
	Validate::isNotEmpty(*this);
	std::unique_ptr<std::string> item = std::move(values[--_size]);
	values[_size].reset();
	return item;
}

std::unique_ptr<std::string> Stack::peek() {
	Validate::isNotEmpty(*this);
	return std::make_unique<std::string>(*values[_size - 1]);
}