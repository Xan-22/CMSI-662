#include <memory>
#include <string>
#include <iostream>

class Data {
    int hidden;
public:
    Data(int num) : hidden(num) {}
    Data(Data const *other) : hidden(other->hidden) {}
};

class Container {
    std::shared_ptr<Data> data;
public:
    Container(int value) : data(new Data(value)) {
        std::cout << "Data inside Container: " << data.get() << std::endl;
    }
    ~Container() { data.reset(); }
    std::shared_ptr<Data> get() const { return std::shared_ptr(data); }
};

int main(void) {
    auto container = std::make_unique<Container>(42);
    auto data = container->get();
    container.reset();
    std::cout << "Data outside Container: " << data.get() << std::endl;
}