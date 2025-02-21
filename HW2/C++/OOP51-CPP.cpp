#include <iostream>
#include <string>

class Base {
public:
    virtual void print() const { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void print() const override { std::cout << "Derived\n"; }
};

void noncompliant(Base b) { // THIS IS NON-COMPLIANT CODE
    b.print(); // Prints "Base" because it is copied as the base class
}

void compliant(const Base *b) { // THIS IS COMPLIANT CODE
    b->print(); // Prints "Derived" because it is passed by reference
}

int main(void) {
    Base *a = new Derived();
    a->print(); // Prints "Derived" because it is pointing to the derived object.
    delete a;

    Derived b = Derived();
    noncompliant(b); // Prints "Base" because it is copied as the base class
    compliant(&b); // Prints "Derived" because it is passed by reference
}