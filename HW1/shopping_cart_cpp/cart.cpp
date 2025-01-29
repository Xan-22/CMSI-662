#include <iostream>
#include <map>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <regex>

class ItemName {
public:
    ItemName(const std::string& name) : name(name) {}
    std::string get() const { return name; };
	bool operator<(const ItemName& other) const {
		return name < other.name;
	}
private:
    std::string name;
};

class Quantity {
public:
	Quantity() : quantity(0) {}
    Quantity(int quantity) {
		if (quantity < 0) {
			throw std::invalid_argument("Quantity cannot be negative");
		}
		if (quantity > 999) {
			throw std::invalid_argument("Quantity cannot be greater than 999");
		}
		this->quantity = quantity;
	}
    int get() const { return quantity; }
private:
    int quantity;
};

class CartID {
public:
	CartID() {
		this->id = generate_uuid4();
	}
	std::string get() const { return id; };
private:
	std::string generate_uuid4() {
		// Credit to Copilot for generating most of the code in this method.
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis;

		uint32_t data[4];
		for (auto& d : data) {
			d = dis(gen);
		}

		std::stringstream ss;
		ss << std::hex << std::setfill('0');
		ss << std::setw(8) << data[0]
			<< '-' << std::setw(4) << (data[1] >> 16)
			<< '-' << std::setw(4) << ((data[1] & 0xFFFF) | 0x4000)  // Version 4
			<< '-' << std::setw(4) << ((data[2] >> 16) & 0x0FFF | 0x8000)  // Variant 1
			<< '-' << std::setw(4) << (data[2] & 0xFFFF)
			<< std::setw(8) << data[3];

		return ss.str();
	}

	std::string id;
};

class OwnerID {
public:
	OwnerID() { this->id = ""; }
	OwnerID(const std::string& id) {
		// Must be 3 letters, 5 numbers, 2 letters, a dash, and an A or a Q.
		std::regex regex("^[A-Z]{3}[0-9]{5}[A-Z]{2}-[AQ]$");
		if (!std::regex_match(id, regex)) {
			throw std::invalid_argument("Invalid owner ID format");
		}
		this->id = id;
	}
	std::string get() const { return id; };
private:
	std::string id;
};

class Catalog {
public:
	Catalog() {
		fetchItems();
	}
	void fetchItems() {
		// This is where items would be fetched from a database or API.
		items = {
			{ItemName("apple"), 0.5},
			{ItemName("banana"), 0.25},
			{ItemName("orange"), 0.75},
			{ItemName("grapes"), 1.0},
			{ItemName("pineapple"), 2.0}
		};
	}
	double getItem(const std::string& item) const {
		ItemName item_name(item);
		auto it = items.find(item_name);
		if (it != items.end()) {
			return (it->first, it->second);
		}
		else {
			throw std::invalid_argument("Item not found in catalog");
		}
	}
private:
	std::map<ItemName, double> items;
};

class ShoppingCart {
public:
	ShoppingCart(const std::string& owner_id) {
		this->owner_id = OwnerID(owner_id);
		CartID cart_id = CartID();
	}

    std::string getId() const { return owner_id.get(); }
	std::string getCartId() const { return cart_id.get(); }

    void addItem(const std::string item_name, int amount) {
		ItemName item(item_name);
		Quantity quantity(amount);
		items[item] = quantity;
    }

	void updateItem(const std::string item_name, int amount) {
		ItemName item(item_name);
		Quantity quantity(amount);
		items[item] = quantity;
	}

	void removeItem(const std::string item_name) {
		ItemName item(item_name);
		items.erase(item);
	}

	double getTotalCost() const {
		double total = 0.0;
		//for (const auto& item : items) {
		//	total += item.second.get();
		//}
		return total;
	}


private:
	OwnerID owner_id;
	CartID cart_id;
	std::map<ItemName, Quantity> items;
};

int main() {
	ShoppingCart cart("ABC12345DE-A");
	try {
		ShoppingCart cart2("12345DE-A");
	}
	catch (const std::invalid_argument& e) {
		std::cout << "Successfully caught exception: " << e.what() << std::endl;
	}

    return 0;
}
