#pragma once
#include <memory>
#include <string>

#define MAXIMUM_CAPACITY 65536
#define STARTING_CAPACITY 16

class Stack {
private:
	std::unique_ptr<std::string>* values; // Raw array of smart pointers (as specified)
	unsigned int _capacity;
	unsigned int _size;
public:
	Stack();
	~Stack();
	Stack(Stack&& other) noexcept;
	Stack& operator=(Stack&& other) noexcept;
	Stack(const Stack& other) = delete; // Disable copy constructor
	Stack& operator=(const Stack& other) = delete; // Disable copy assignment
	unsigned int size() const;
	bool isFull() const;
	bool isAtCapacity() const;
	bool isEmpty() const;
	void push(std::string);
	std::unique_ptr<std::string> pop();
	std::unique_ptr<std::string> peek();
};