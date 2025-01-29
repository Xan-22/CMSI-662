#include "cart.h"
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
		// Credit to happy_sisyphus on Stackoverflow: https://stackoverflow.com/a/60198074
		// The alternative to this would be to use an external library like Boost.
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis(0, 15); // 0-f
		std::uniform_int_distribution<uint32_t> encoding_bits(8, 11); // 8-b

		std::stringstream ss;
		int i;
		ss << std::hex;
		for (i = 0; i < 8; i++) {
			ss << dis(gen);
		}
		ss << "-";
		for (i = 0; i < 4; i++) {
			ss << dis(gen);
		}
		ss << "-4";
		for (i = 0; i < 3; i++) {
			ss << dis(gen);
		}
		ss << "-" << encoding_bits(gen);
		for (i = 0; i < 3; i++) {
			ss << dis(gen);
		}
		ss << "-";
		for (i = 0; i < 12; i++) {
			ss << dis(gen);
		}
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

struct ShoppingCart::ShoppingCartData {
	OwnerID owner_id;
	CartID cart_id;
	std::map<ItemName, Quantity> items;
};

ShoppingCart::ShoppingCart(const std::string& owner_id) {
	data = std::make_unique<ShoppingCartData>(OwnerID(owner_id), CartID(), std::map<ItemName, Quantity>());
}
ShoppingCart::~ShoppingCart() = default;

// Copy Constructor
ShoppingCart::ShoppingCart(const ShoppingCart& other) {
	data = std::make_unique<ShoppingCartData>(other.data->owner_id, other.data->cart_id, other.data->items);
};

// Move Constructor
ShoppingCart::ShoppingCart(ShoppingCart&& other) noexcept = default;

// Copy Assignment
ShoppingCart& ShoppingCart::operator=(const ShoppingCart& other) {
	data = std::make_unique<ShoppingCartData>(other.data->owner_id, other.data->cart_id, other.data->items);
	return *this;
};

// Move Assignment
ShoppingCart& ShoppingCart::operator=(ShoppingCart&& other) noexcept = default;

std::string ShoppingCart::getId() const { return data->owner_id.get(); }
std::string ShoppingCart::getCartId() const { return data->cart_id.get(); }
std::map <std::string, int> ShoppingCart::getItems() const {
		std::map <std::string, int> copied_items;
		for (const auto& item : data->items) {
			copied_items[item.first.get()] = item.second.get();
		}
		return copied_items; 
	}

void ShoppingCart::addItem(const std::string item_name, int amount) {
		ItemName item(item_name);
		Quantity quantity(amount);
		data->items[item] = quantity;
    }

void ShoppingCart::updateItem(const std::string item_name, int amount) {
		ItemName item(item_name);
		Quantity quantity(amount);
		data->items[item] = quantity;
	}

void ShoppingCart::removeItem(const std::string item_name) {
		ItemName item(item_name);
		data->items.erase(item);
	}

double ShoppingCart::getTotalCost() const {
		double total = 0.0;
		//for (const auto& item : items) {
		//	total += item.second.get();
		//}
		return total;
	}